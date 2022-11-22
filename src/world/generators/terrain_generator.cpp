#include "./terrain_generator.hpp"

#include <cmath>
#include <utility>
#include <functional>
#include <chrono>
#include <random>
#include <libtcod.hpp>
#include "./lib/poisson_disk_sampling.hpp"
#include "./lib/gal/fortune_algorithm.hpp"
#include "./lib/bezier.hpp"

// TEMP
#include <iostream>
// TEMP

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace dl
{
  std::vector<int> TerrainGenerator::generate(const int width, const int height, const int seed)
  {
    // TEMP
    m_lua.load("generators/terrain.lua");
    // TEMP

    std::cout << "=============================\n";
    std::cout << "= STARTING WORLD GENERATION =\n";
    std::cout << "=============================\n\n";
    std::cout << "SEED: " << seed << "\n\n";

    std::vector<int> tiles(width * height);

    auto start = std::chrono::high_resolution_clock::now();

    std::cout << "[*] Generating world silhouette...\n";

    m_generate_silhouette(tiles, width, height, seed);

    std::cout << "[*] Adjusting islands...\n";

    // Get a vector with remaining islands in crescent order (last element is the main island)
    auto islands = m_get_islands(tiles, width, height);
    auto& main_island = islands.back();

    std::cout << "[*] Identifying coastline...\n";

    auto coastline = m_get_coastline(tiles, width, height, main_island);

    std::cout << "[*] Identifying bays...\n";

    auto bays = m_get_bays(coastline, main_island, width, height);

    std::cout << "[*] Building main island geometry...\n";

    m_build_island_structure(main_island, width, height);

    std::cout << "[*] Generating main river...\n";

    m_generate_main_river(main_island, bays, tiles, width, height, seed);

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

    const auto draw_features = m_lua.get_variable<bool>("draw_features");

    if (draw_features)
    {
      // Draw structure
      const auto draw_structure = m_lua.get_variable<bool>("draw_structure");
      if (draw_structure)
      {
        for (auto site : main_island.structure.diagram.get_sites())
        {
          m_draw_big_point(site.point.convert(width, height), 5, tiles, width);
        }
        /* for (const auto& site : main_island.structure.sites) */
        /* for (const auto index : main_island.structure.coast_indexes) */
        /* { */
          /* m_draw_big_point(main_island.structure.sites[index].center, 5, tiles, width); */

          /* for (const auto& edge : main_island.structure.sites[index].edges) */
          /* { */
          /*   m_draw_line(edge.first, edge.second, 5, tiles, width, height); */
          /* } */

          /* m_draw_big_point(site.center, 5, tiles, width); */

          /* for (const auto& edge : site.edges) */
          /* { */
          /*   m_draw_line(edge.first, edge.second, 5, tiles, width, height); */
          /* } */
        /* } */
      }

      // Draw coastline
      const auto draw_coastline = m_lua.get_variable<bool>("draw_coastline");
      if (draw_coastline)
      {
        for (const auto& coastline_point : coastline)
        {
          m_draw_point(coastline_point, 4, tiles, width);
        }
      }

      // Draw bays
      const auto draw_bays = m_lua.get_variable<bool>("draw_bays");
      if (draw_bays)
      {
        for (const auto& bay : bays)
        {
          m_draw_big_point(bay, 0, tiles, width);
        }
      }
    }

    std::cout << "[*] World generation finished! It took " << duration.count() << " milliseconds\n\n";

    return tiles;
  }

  void TerrainGenerator::m_generate_silhouette(std::vector<int>& tiles, const int width, const int height, const int seed)
  {
    const auto simplex_freq = m_lua.get_variable<float>("simplex_freq");
    const auto simplex_octaves = m_lua.get_variable<int>("simplex_octaves");
    const auto simplex_lacunarity = m_lua.get_variable<float>("simplex_lacunarity");
    const auto simplex_gain = m_lua.get_variable<float>("simplex_gain");
    const auto simplex_weighted_strength = m_lua.get_variable<float>("simplex_weighted_strength");

    m_noise.SetSeed(seed);
    m_noise.SetNoiseType(FastNoiseLite::NoiseType::NoiseType_OpenSimplex2S);
    m_noise.SetRotationType3D(FastNoiseLite::RotationType3D_ImproveXYPlanes);
    m_noise.SetFrequency(simplex_freq);
    m_noise.SetFractalType(FastNoiseLite::FractalType::FractalType_FBm);
    m_noise.SetFractalOctaves(simplex_octaves);
    m_noise.SetFractalLacunarity(simplex_lacunarity);
    m_noise.SetFractalGain(simplex_gain);
    m_noise.SetFractalWeightedStrength(simplex_weighted_strength);

    const auto tier_land = m_lua.get_variable<float>("tier_land");

    for (int j = 0; j < height; ++j)
    {
      for (int i = 0; i < width; ++i)
      {
        const float gradient = m_get_rectangle_gradient_value(i, j, width, height); 

        const float noise_value =  m_noise.GetNoise(static_cast<float>(i), static_cast<float>(j)) - gradient;

        int tile_value;

        if (noise_value > tier_land)
        {
          tile_value = 2;
        }
        else
        {
          tile_value = 0;
        }

        tiles[j*width + i] = tile_value;
      }
    }

    // Remove lakes
    bool has_flooded_ocean = false;

    for (int j = 0; j < height && !has_flooded_ocean; ++j)
    {
      for (int i = 0; i < width && !has_flooded_ocean; ++i)
      {
        if (i != 0 && j != 0)
        {
          continue;
        }

        const auto value = tiles[j*width + i];

        if (value == 0)
        {
          if (j == 0 || i == 0)
          {
            m_flood_fill(1, i, j, tiles, width, height);
            has_flooded_ocean = true;
          }
        }
      }
    }

    // Replace inland water with grass
    for (int j = 0; j < height; ++j)
    {
      for (int i = 0; i < width; ++i)
      {
        if (tiles[j*width + i] == 0)
        {
          tiles[j*width + i] = 2;
        }
      }
    }
  }

  float TerrainGenerator::m_get_rectangle_gradient_value(const int x, const int y, const int width, const int height)
  {
    auto distance_to_edge = std::min(abs(x - width), x);
    distance_to_edge = std::min(distance_to_edge, abs(y - height));
    distance_to_edge = std::min(distance_to_edge, y) * 2;
    return 1.f - static_cast<float>(distance_to_edge) / (width / 2.0f);
  }

  std::vector<IslandData> TerrainGenerator::m_get_islands(std::vector<int>& tiles, const int width, const int height)
  {
    auto all_islands = m_get_island_queue(tiles, width, height);

    const auto number_of_islands_to_keep = m_lua.get_variable<std::uint32_t>("islands_to_keep");

    while (all_islands.size() > number_of_islands_to_keep)
    {
      const auto& current_island = all_islands.top();

      for (const auto& coord : current_island.points)
      {
        // Set island tiles to water to remove them
        tiles[coord.y*width + coord.x] = 1;
      }

      all_islands.pop();
    }

    // Keep remaining islands in a vector
    // As we ordered the priority queue in crescent order, the last
    // island will be the main island
    std::vector<IslandData> islands(all_islands.size());

    while(!all_islands.empty())
    {
      islands.push_back(std::move(const_cast<IslandData&>(all_islands.top())));
      all_islands.pop();
    }

    return islands;
  }

  IslandQueue TerrainGenerator::m_get_island_queue(const std::vector<int>& tiles, const int width, const int height)
  {
    std::vector<int> mask(width * height, 1);
    IslandQueue islands{};

    for (int j = 0; j < height; ++j)
    {
      for (int i = 0; i < width; ++i)
      {
        const auto tile_value = tiles[j*width + i];
        const auto mask_value = mask[j*width + i];

        // Not water and not masked
        if (tile_value != 1 && mask_value == 1)
        {
          const auto island_data = m_get_island(tiles, mask, i, j, width, height);
          islands.push(island_data);
        }
      }
    }

    return islands;
  }

  IslandData TerrainGenerator::m_get_island(const std::vector<int>& tiles, std::vector<int>& mask, const int x, const int y, const int width, const int height)
  {
    assert(m_valid_coord(x, y, width, height) && "Island coordinates are not valid");

    IslandData island_data;
    island_data.mask.resize(width*height, 1);

    std::queue<Point<int>> coord_queue;

    coord_queue.push(Point<int>(x, y));
    island_data.top_left.x = x;
    island_data.top_left.y = y;
    island_data.bottom_right.x = x;
    island_data.bottom_right.y = y;

    while (!coord_queue.empty())
    {
      auto coord = coord_queue.front();

      const int x0 = coord.x;
      const int y0 = coord.y;

      island_data.mask[y0*width + x0] = 2;
      island_data.points.push_back(Point<int>(x0, y0));
      island_data.top_left.x = std::min(x0, island_data.top_left.x);
      island_data.top_left.y = std::min(y0, island_data.top_left.y);
      island_data.bottom_right.x = std::max(x0, island_data.bottom_right.x);
      island_data.bottom_right.y = std::max(y0, island_data.bottom_right.y);

      coord_queue.pop();

      // Top coord
      const auto yt = y0 + 1;
      const auto top_coord = yt*width + x0;
      if (m_valid_coord(x0, yt, width, height) && tiles[top_coord] != 1 && mask[top_coord] == 1)
      {
        mask[top_coord] = 2;
        coord_queue.push(Point<int>(x0, yt));
      }

      // Bottom coord
      const auto yb = y0 - 1;
      const auto bottom_coord = yb*width + x0;
      if (m_valid_coord(x0, yb, width, height) && tiles[bottom_coord] != 1 && mask[bottom_coord] == 1)
      {
        mask[bottom_coord] = 2;
        coord_queue.push(Point<int>(x0, yb));
      }

      // Left coord
      const auto xl = x0 - 1;
      const auto left_coord = y0*width + xl;
      if (m_valid_coord(xl, y0, width, height) && tiles[left_coord] != 1 && mask[left_coord] == 1)
      {
        mask[left_coord] = 2;
        coord_queue.push(Point<int>(xl, y0));
      }

      // Right coord
      const auto xr = x0 + 1;
      const auto right_coord = y0*width + xr;
      if (m_valid_coord(xr, y0, width, height) && tiles[right_coord] != 1 && mask[right_coord] == 1)
      {
        mask[right_coord] = 2;
        coord_queue.push(Point<int>(xr, y0));
      }
    }

    return island_data;
  }

  bool TerrainGenerator::m_valid_coord(const int x, const int y, const int width, const int height)
  {
    return !(x < 0 || y < 0 || x >= width || y >= height);
  }

  bool TerrainGenerator::m_valid_point(const Point<int>& point, const int width, const int height)
  {
    return !(point.x < 0 || point.y < 0 || point.x >= width || point.y >= height);
  }

  std::vector<Point<int>> TerrainGenerator::m_get_coastline(const std::vector<int>& tiles, const int width, const int height, const IslandData& island)
  {
    std::vector<Point<int>> coastline;
    std::vector<int> mask(width * height, 1);
    const std::size_t mask_size = mask.size();

    const auto first_point = m_get_first_coastline_point(tiles, width, height, island);
    mask[first_point.y*width + first_point.x] = 2;

    // TODO: Handle not found
    if (first_point.x == 0 && first_point.y == 0)
    {
      return coastline;
    }

    std::queue<Point<int>> point_queue;
    point_queue.push(first_point);

    while(!point_queue.empty())
    {
      const auto current_point = point_queue.front();
      point_queue.pop();

      coastline.push_back(current_point);
      mask[current_point.y*width + current_point.x] = 2;

      const auto left_point = current_point.left();
      const auto right_point = current_point.right();
      const auto bottom_point = current_point.bottom();
      const auto top_point = current_point.top();

      auto manipulate_point = [width, height, mask_size, &mask, &point_queue, &tiles](const Point<int>& point)
      {
        if (m_valid_point(point, width, height) && m_is_coast_point(point, width, height, tiles) && mask[point.y*width + point.x] == 1)
        {
          int neighbours = 0;

          const std::size_t top_left_coord = (point.y - 1)*width + point.x - 1;
          const std::size_t top_coord = (point.y - 1)*width + point.x;
          const std::size_t top_right_coord = (point.y - 1)*width + point.x + 1;
          const std::size_t left_coord = point.y*width + point.x - 1;
          const std::size_t right_coord = point.y*width + point.x + 1;
          const std::size_t bottom_left_coord = (point.y + 1)*width + point.x - 1;
          const std::size_t bottom_coord = (point.y + 1)*width + point.x;
          const std::size_t bottom_right_coord = (point.y + 1)*width + point.x + 1;

          // Top left
          if (top_left_coord < mask_size && mask[top_left_coord] == 2)
          {
            ++neighbours;
          }
          if (top_coord < mask_size && mask[top_coord] == 2)
          {
            ++neighbours;
          }
          if (top_right_coord < mask_size && mask[top_right_coord] == 2)
          {
            ++neighbours;
          }
          if (left_coord < mask_size && mask[left_coord] == 2)
          {
            ++neighbours;
          }
          if (right_coord < mask_size && mask[right_coord] == 2)
          {
            ++neighbours;
          }
          if (bottom_left_coord < mask_size && mask[bottom_left_coord] == 2)
          {
            ++neighbours;
          }
          if (bottom_coord < mask_size && mask[bottom_coord] == 2)
          {
            ++neighbours;
          }
          if (bottom_right_coord < mask_size && mask[bottom_right_coord] == 2)
          {
            ++neighbours;
          }

          if (neighbours < 3)
          {
            point_queue.push(point);
          }
        }
      };

      manipulate_point(left_point);
      manipulate_point(right_point);
      manipulate_point(bottom_point);
      manipulate_point(top_point);
    }

    return coastline;
  }

  bool TerrainGenerator::m_contains_island_area(const Point<int>& point, const float segment_width_h, const float segment_width_v, const int width, const int height, const std::vector<int>& island_mask)
  {
    const auto top_left_x = std::max(static_cast<int>(point.x - std::round(segment_width_h)), 0);
    const auto top_left_y = std::max(static_cast<int>(point.y - std::round(segment_width_v)), 0);
    const auto bottom_right_x = std::min(static_cast<int>(point.x + std::round(segment_width_h)), static_cast<int>(width));
    const auto bottom_right_y = std::min(static_cast<int>(point.y + std::round(segment_width_v)), static_cast<int>(height));
    
    for (auto i = top_left_x; i < bottom_right_x; ++i)
    {
      for (auto j = top_left_y; j < bottom_right_y; ++j)
      {
        if (island_mask[j*width + i] != 1)
        {
          return true;
        }
      }
    }

    return false;
  }

  void TerrainGenerator::m_flood_fill(const int value, const int x, const int y, std::vector<int>& tiles, const int width, const int height)
  {
    assert(m_valid_coord(x, y, width, height));

    const int original_value = tiles[y*width + x];
    std::queue<Point<int>> coord_queue;
    std::vector<int> mask(width * height, 1);

    coord_queue.push(Point<int>(x, y));
    tiles[y*width + x] = value;

    while (!coord_queue.empty())
    {
      auto coord = coord_queue.front();
      const int x0 = coord.x;
      const int y0 = coord.y;

      coord_queue.pop();

      // Top coord
      const auto yt = y0 + 1;
      const auto top_coord = yt*width + x0;
      if (m_valid_coord(x0, yt, width, height) && tiles[top_coord] == original_value && mask[top_coord] == 1)
      {
        mask[top_coord] = 2;
        tiles[top_coord] = value;
        coord_queue.push(Point<int>(x0, yt));
      }

      // Bottom coord
      const auto yb = y0 - 1;
      const auto bottom_coord = yb*width + x0;
      if (m_valid_coord(x0, yb, width, height) && tiles[bottom_coord] == original_value && mask[bottom_coord] == 1)
      {
        mask[bottom_coord] = 2;
        tiles[bottom_coord] = value;
        coord_queue.push(Point<int>(x0, yb));
      }

      // Left coord
      const auto xl = x0 - 1;
      const auto left_coord = y0*width + xl;
      if (m_valid_coord(xl, y0, width, height) && tiles[left_coord] == original_value && mask[left_coord] == 1)
      {
        mask[left_coord] = 2;
        tiles[left_coord] = value;
        coord_queue.push(Point<int>(xl, y0));
      }

      // Right coord
      const auto xr = x0 + 1;
      const auto right_coord = y0*width + xr;
      if (m_valid_coord(xr, y0, width, height) && tiles[right_coord] == original_value && mask[right_coord] == 1)
      {
        mask[right_coord] = 2;
        tiles[right_coord] = value;
        coord_queue.push(Point<int>(xr, y0));
      }
    }
  }

  Point<int> TerrainGenerator::m_get_first_coastline_point(const std::vector<int>& tiles, const int width, const int height, const IslandData& island)
  {
    for (const auto& point : island.points)
    {
      bool left_candidate = false;
      bool right_candidate = false;
      bool bottom_candidate = false;
      bool top_candidate = false;

      const auto left_point = point.left();
      const auto right_point = point.right();
      const auto bottom_point = point.bottom();
      const auto top_point = point.top();

      if (m_is_water(left_point, width, height, tiles))
      {
        left_candidate = true;
      }
      if (m_is_water(right_point, width, height, tiles))
      {
        right_candidate = true;
      }
      if (m_is_water(bottom_point, width, height, tiles))
      {
        bottom_candidate = true;
      }
      if (m_is_water(top_point, width, height, tiles))
      {
        top_candidate = true;
      }

      if (!left_candidate && !right_candidate && !bottom_candidate && !top_candidate)
      {
        continue;
      }

      bool is_coast = true;

      if (left_candidate)
      {
        is_coast = true;

        for (auto x = point.x - 1; x > 0; --x)
        {
          if (tiles[point.y*width + x] != 1)
          {
            is_coast = false;
          }
        }
      }
      if (right_candidate && !is_coast)
      {
        is_coast = true;

        for (auto x = point.x + 1; x < width - 1; ++x)
        {
          if (tiles[point.y*width + x] != 1)
          {
            is_coast = false;
          }
        }
      }
      if (bottom_candidate && !is_coast)
      {
        is_coast = true;

        for (auto y = point.y - 1; y > 0; --y)
        {
          if (tiles[y*width + point.x] != 1)
          {
            is_coast = false;
          }
        }
      }
      if (top_candidate && !is_coast)
      {
        is_coast = true;

        for (auto y = point.y + 1; y < height - 1; ++y)
        {
          if (tiles[y*width + point.x] != 1)
          {
            is_coast = false;
          }
        }
      }

      if (is_coast)
      {
        return point;
      }
    }

    // Error: could't find coastline point
    return Point<int>(width, height);
  }

  int TerrainGenerator::m_get_point_value(const Point<int>& point, const int width, const std::vector<int>& tiles)
  {
    return tiles[point.y*width + point.x];
  }

  bool TerrainGenerator::m_is_coast_point(const Point<int>& point, const int width, const int height, const std::vector<int>& tiles)
  {
    // If point is water
    if (m_get_point_value(point, width, tiles) == 1)
    {
      return false;
    }

    // Test 8 directions from point
    if (m_is_water(point.left(), width, height, tiles))
    {
      return true;
    }
    if (m_is_water(point.right(), width, height, tiles))
    {
      return true;
    }
    if (m_is_water(point.bottom(), width, height, tiles))
    {
      return true;
    }
    if (m_is_water(point.top(), width, height, tiles))
    {
      return true;
    }
    if (m_is_water(point.top_left(), width, height, tiles))
    {
      return true;
    }
    if (m_is_water(point.top_right(), width, height, tiles))
    {
      return true;
    }
    if (m_is_water(point.bottom_right(), width, height, tiles))
    {
      return true;
    }
    if (m_is_water(point.bottom_left(), width, height, tiles))
    {
      return true;
    }

    return false;
  }

  bool TerrainGenerator::m_is_water(const Point<int>& point, const int width, const int height, const std::vector<int>& tiles)
  {
    return m_valid_point(point, width, height) && m_get_point_value(point, width, tiles) == 1;
  }

  std::vector<Point<int>> TerrainGenerator::m_get_bays(std::vector<Point<int>>& coastline, const IslandData& island, const int width, const int height)
  {
    std::vector<Point<int>> bays;

    const auto minimum_area = m_lua.get_variable<int>("coast_min_area");
    const auto points_min_distance = m_lua.get_variable<int>("coast_points_min_distance");
    const auto points_max_distance = m_lua.get_variable<int>("coast_points_max_distance");

    std::vector<std::pair<Point<int>, Point<int>>> candidates;
    Point<int>* coast_point_a = nullptr;
    Point<int>* coast_point_b = nullptr;

    std::vector<int> area_mask(width*height, 1);

    for (std::size_t n = 0; n < coastline.size(); ++n)
    {
      bool found_candidate = false;

      for (std::size_t m = 0; m < coastline.size(); ++m)
      {
        if (n == m)
        {
          continue;
        }

        auto& point_a = coastline[n];
        auto& point_b = coastline[m];

        const float distance = m_distance(point_a, point_b);

        if (distance < points_min_distance || distance > points_max_distance)
        {
          continue;
        }

        if (!m_has_land_intersection(point_a, point_b, island.mask, width, 1))
        {
          found_candidate = true;
          coast_point_a = &point_a;
          coast_point_b = &point_b;
        }
        else if (found_candidate && coast_point_a != nullptr && coast_point_b != nullptr)
        {
          const auto bay_data = m_get_bay_data(*coast_point_a, *coast_point_b, island.mask, width, height, minimum_area, area_mask, 1);

          if (bay_data.area >= minimum_area)
          {
            bays.push_back(bay_data.point);
          }

          found_candidate = false;
        }
      }
    }

    return bays;
  }

  bool TerrainGenerator::m_has_land_intersection(const Point<int>& point_a, const Point<int>& point_b, const std::vector<int>& tiles, const int width, const int water_value)
  {
    const int middle_x = (point_a.x + point_b.x)/2;
    const int middle_y = (point_a.y + point_b.y)/2;

    if (tiles[middle_y*width + middle_x] != water_value)
    {
      return true;
    }

    int tolerance = 3;
    int x = point_a.x;
    int y = point_a.y;

    TCOD_bresenham_data_t bresenham_data;
    TCOD_line_init_mt(point_a.x, point_a.y, point_b.x, point_b.y, &bresenham_data);

    do
    {
      if ((x == static_cast<int>(point_a.x) && y == static_cast<int>(point_a.y)) || (x == static_cast<int>(point_b.x) && y == static_cast<int>(point_b.y)))
      {
        continue;
      }

      if (tiles[y*width + x] != water_value)
      {
        --tolerance;
      }

      if (tolerance <= 0)
      {
        return true;
      }
    }
    while (!TCOD_line_step_mt(&x, &y, &bresenham_data));

    return false;
  }

  BayData TerrainGenerator::m_get_bay_data(const Point<int>& point_a, const Point<int>& point_b, const std::vector<int>& tiles, const int width, const int height, const int minimum, std::vector<int>& mask, const int water_value)
  {
    Point<int> bay_point(point_a.x, point_b.y);

    {
      int x = point_a.x;
      int y = point_a.y;

      TCOD_bresenham_data_t bresenham_data;
      TCOD_line_init_mt(x, y, point_b.x, point_b.y, &bresenham_data);

      do
      {
        if(mask[y*width + x] == 2)
        {
          return BayData(0, bay_point);
        }

        mask[y*width + x] = 2;
      }
      while (!TCOD_line_step_mt(&x, &y, &bresenham_data));
    }

    int start_x, start_y;
    const int middle_x = (point_a.x + point_b.x)/2;
    const int middle_y = (point_a.y + point_b.y)/2;

    // Find perpendicular line
    {
      const int delta_x = point_b.x - point_a.x;
      const int delta_y = point_b.y - point_a.y;

      int steps_direction_1 = 0;
      int steps_direction_2 = 0;

      Point<int> candidate_a(middle_x, middle_y);
      Point<int> candidate_b(middle_x, middle_y);

      // Ignore vertical and horizontal lines for simplicity
      if (delta_x == 0 || delta_y == 0)
      {
        return BayData(0, bay_point);
      }
      else
      {
        const float a = (delta_y) / static_cast<float>((delta_x));
        const float b = -1.f;
        const float d = b*middle_x - a*middle_y;

        /* auto perpendicular_x = [a, b, d](int y) */
        /* { */
        /*   (a*y + d) / static_cast<float>(b); */
        /* }; */

        // Find y in function of x
        auto perpendicular_y = [a, b, d](int x)
        {
          return (b*x - d) / static_cast<float>(a);
        };

        for (auto px = middle_x + 1; px < width; ++px)
        {
          ++steps_direction_1;
          const auto py = static_cast<int>(std::round(perpendicular_y(px)));

          if (py >= 0 && static_cast<int>(py) < height && tiles[py*width + px] != water_value)
          {
            if (mask[py*width + px] != 1)
            {
              return BayData(0, bay_point);
            }

            // Get previous point
            candidate_a.x = px - 1;
            candidate_a.y = static_cast<int>(std::round(perpendicular_y(px - 1)));
            break;
          }
          else if (py < 2 || static_cast<int>(py) >= height)
          {
            steps_direction_1 = height + 1;
            break;
          }
        }
        for (auto px = middle_x - 1; px > 0; --px)
        {
          ++steps_direction_2;
          const auto py = static_cast<int>(std::round(perpendicular_y(px)));

          if (py >= 0 && static_cast<int>(py) < height && tiles[py*width + px] != water_value)
          {
            if (mask[py*width + px] != 1)
            {
              return BayData(0, bay_point);
            }

            candidate_b.x = px + 1;
            candidate_b.y = static_cast<int>(std::round(perpendicular_y(px + 1)));
            break;
          }
          else if (py < 2 || static_cast<int>(py) >= height || px < 2)
          {
            steps_direction_2 = height + 1;
            break;
          }
        }
      }

      if (steps_direction_1 < steps_direction_2)
      {
        start_x = candidate_a.x;
        start_y = candidate_a.y;
        bay_point = candidate_a;
      }
      else
      {
        start_x = candidate_b.x;
        start_y = candidate_b.y;
        bay_point = candidate_b;
      }
    }

    const auto delta_candidate_x = std::abs(static_cast<int>(start_x) - static_cast<int>(middle_x));
    const auto delta_candidate_y = std::abs(static_cast<int>(start_y) - static_cast<int>(middle_y));
    const auto min_land_distance_delta = m_lua.get_variable<int>("min_land_distance_delta");

    // Ignore when land is found very close to the center point
    if (delta_candidate_x < min_land_distance_delta && delta_candidate_y < min_land_distance_delta)
    {
      return BayData(0, bay_point);
    }

    // Flood fill ocean area between line and land
    int area = 0;
    std::queue<Point<int>> coord_queue;

    coord_queue.push(Point<int>(start_x, start_y));
    mask[start_y*width + start_x] = 2;
    area++;

    while (!coord_queue.empty())
    {
      auto coord = coord_queue.front();
      const int x0 = coord.x;
      const int y0 = coord.y;

      coord_queue.pop();

      // Top coord
      const auto yt = y0 + 1;
      const auto top_coord = yt*width + x0;
      if (m_valid_coord(x0, yt, width, height) && tiles[top_coord] == water_value && mask[top_coord] == 1)
      {
        mask[top_coord] = 2;
        coord_queue.push(Point<int>(x0, yt));
        area++;
      }

      // Bottom coord
      const auto yb = y0 - 1;
      const auto bottom_coord = yb*width + x0;
      if (m_valid_coord(x0, yb, width, height) && tiles[bottom_coord] == water_value && mask[bottom_coord] == 1)
      {
        mask[bottom_coord] = 2;
        coord_queue.push(Point<int>(x0, yb));
        area++;
      }

      // Left coord
      const auto xl = x0 - 1;
      const auto left_coord = y0*width + xl;
      if (m_valid_coord(xl, y0, width, height) && tiles[left_coord] == water_value && mask[left_coord] == 1)
      {
        mask[left_coord] = 2;
        coord_queue.push(Point<int>(xl, y0));
        area++;
      }

      // Right coord
      const auto xr = x0 + 1;
      const auto right_coord = y0*width + xr;
      if (m_valid_coord(xr, y0, width, height) && tiles[right_coord] == water_value && mask[right_coord] == 1)
      {
        mask[right_coord] = 2;
        coord_queue.push(Point<int>(xr, y0));
        area++;
      }

      if (area >= minimum)
      {
        break;
      }
    }

    return BayData(area, bay_point);
  }

  void TerrainGenerator::m_build_island_structure(IslandData& island, const int width, const int height)
  {
    assert(island.points.size() > 0 && "Main island size is empty");

    std::array<float, 2> min_point = {static_cast<float>(island.top_left.x - 20), static_cast<float>(island.top_left.y - 20)};
    std::array<float, 2> max_point = {static_cast<float>(island.bottom_right.x + 20), static_cast<float>(island.bottom_right.y + 20)};

    const auto poisson_disk_sampling_radius = m_lua.get_variable<float>("poisson_disk_sampling_radius");
    const auto poisson_points = thinks::PoissonDiskSampling(poisson_disk_sampling_radius, min_point, max_point);
    std::vector<gal::Vector2<double>> points{};

    // Normalize points to [0.0, 1.0]
    for (const auto& point : poisson_points)
    {
      double normalized_x = point[0] / static_cast<double>(width);
      double normalized_y = point[1] / static_cast<double>(height);

      points.push_back(gal::Vector2{normalized_x, normalized_y});
    }

    auto algorithm = gal::FortuneAlgorithm<double>(points);
    algorithm.construct();

    island.structure.diagram = algorithm.get_diagram();
    auto& diagram = island.structure.diagram;

    for (const auto& site : diagram.get_sites())
    {
      const auto center = site.point.convert(width, height);

      // Center is outside island
      if (island.mask[center.y*width + center.x] == 1)
      {
        continue;
      }

      const auto face = site.face;
      auto half_edge = face->outerComponent;

      if (half_edge == nullptr)
      {
        continue;
      }

      while (half_edge->prev != nullptr)
      {
        half_edge = half_edge->prev;

        if (half_edge == face->outerComponent)
        {
          break;
        }
      }

      const auto start = half_edge;
      bool is_coast = false;

      while (half_edge != nullptr)
      {
        if (half_edge->origin != nullptr && half_edge->destination != nullptr)
        {
          const auto origin = half_edge->origin->point.convert(width, height);
          const auto destination = half_edge->destination->point.convert(width, height);

          // If any of the edges points lays on water annotate it as coast
          if (island.mask[origin.y*width + origin.x] == 1 || island.mask[destination.y*width + destination.y] == 1)
          {
            is_coast = true;
          }
        }

        half_edge = half_edge->next;

        if (half_edge == start)
        {
          break;
        }
      }

      // Re check if it's a coast site if any of the edges points lays on water
      if (!is_coast)
      {
        is_coast = m_center_is_coast(center, island.mask, width, height);
      }

      if (is_coast)
      {
        island.structure.coast_sites.push_back(&site);
      }
      else
      {
        island.structure.land_sites.push_back(&site);
      }
    }
  }

  bool TerrainGenerator::m_center_is_coast(const Point<int>& center, const std::vector<int>& island_mask, const int width, const int height)
  {
    assert(island_mask[center.y*width + center.x] == 2 && "Center must not be in water");

    int distance_to_water = 0;

    // Check to the left of center
    for (int n = 0; (center.x - n) >= 0; ++n)
    {
      if (island_mask[center.y*width + (center.x - n)] == 1)
      {
        distance_to_water = n;
        break;
      }
    }
    // Check to the right of center
    for (int n = 0; (center.x + n) < width; ++n)
    {
      if (island_mask[center.y*width + (center.x + n)] == 1)
      {
        distance_to_water = std::min(distance_to_water, n);
        break;
      }
    }
    // Check to the bottom of center
    for (int n = 0; (center.y + n) < height; ++n)
    {
      if (island_mask[(center.y + n) * width + center.x] == 1)
      {
        distance_to_water = std::min(distance_to_water, n);
        break;
      }
    }
    // Check to the top of center
    for (int n = 0; (center.y - n) >= 0; ++n)
    {
      if (island_mask[(center.y - n) * width + center.x] == 1)
      {
        distance_to_water = std::min(distance_to_water, n);
        break;
      }
    }
    // Check to the top left of center
    for (int n = 0; (center.y - n) >= 0 && (center.x - n) >= 0; ++n)
    {
      if (island_mask[(center.y - n) * width + (center.x - n)] == 1)
      {
        distance_to_water = std::min(distance_to_water, n);
        break;
      }
    }
    // Check to the top right of center
    for (int n = 0; (center.y - n) >= 0 && (center.x + n) < width; ++n)
    {
      if (island_mask[(center.y - n) * width + (center.x + n)] == 1)
      {
        distance_to_water = std::min(distance_to_water, n);
        break;
      }
    }
    // Check to the bottom right of center
    for (int n = 0; (center.y + n) < height && (center.x + n) < width; ++n)
    {
      if (island_mask[(center.y + n) * width + (center.x + n)] == 1)
      {
        distance_to_water = std::min(distance_to_water, n);
        break;
      }
    }
    // Check to the bottom left of center
    for (int n = 0; (center.y + n) < height && (center.x - n) >= 0; ++n)
    {
      if (island_mask[(center.y + n) * width + (center.x - n)] == 1)
      {
        distance_to_water = std::min(distance_to_water, n);
        break;
      }
    }

    const auto coast_length = m_lua.get_variable<int>("coast_length");
    if (distance_to_water <= coast_length)
    {
      return true;
    }

    return false;
  }

  void TerrainGenerator::m_generate_main_river(IslandData& island, std::vector<Point<int>>& bays, std::vector<int>& tiles, const int width, const int height, const int seed)
  {
    const auto [source, mouth] = m_get_river_source_and_mouth(island, bays, width, height, seed);
    auto river = m_get_river_segments(source, mouth, tiles, width, seed);
    /* m_create_meanders(river, tiles, width, height); */

    // Draw river
    for (const auto& segment : river)
    {
      if (segment->next == nullptr)
      {
        break;
      }

      m_draw_line(segment->point, segment->next->point, 1, tiles, width, height);
    }
  }

  std::pair<Point<int>, Point<int>> TerrainGenerator::m_get_river_source_and_mouth(IslandData& island, std::vector<Point<int>>& bays, const int width, const int height, const int seed)
  {
    assert(bays.size() > 0 && "There are no bays identified");
    assert(island.structure.land_sites.size() > 0 && "There are no land sites");

    std::mt19937 rng(seed);
    std::uniform_int_distribution<unsigned int> land_indexes_dist(0, island.structure.land_sites.size() - 1);
    std::uniform_int_distribution<unsigned int> bay_indexes_dist(0, bays.size() - 1);
    const auto min_source_mouth_distance_x = m_lua.get_variable<int>("min_source_mouth_distance_x");
    const auto min_source_mouth_distance_y = m_lua.get_variable<int>("min_source_mouth_distance_y");

    auto source_site = island.structure.land_sites[0];
    auto source_point = source_site->point.convert(width, height);
    auto& mouth_point = bays[0];

    bool found_river_points = false;
    bool has_reached_first_limit = false;
    bool has_reached_second_limit = false;
    std::size_t tries = 0;

    // Check if line crosses sea water to avoid unrealistic rivers
    auto intersects_water = [&island, width](const Point<int>& source, const Point<int>& mouth)
    {
      int tolerance = 4;
      int x = source.x;
      int y = source.y;

      TCOD_bresenham_data_t bresenham_data;
      TCOD_line_init_mt(x, y, mouth.x, mouth.y, &bresenham_data);

      do
      {
        if(island.mask[y*width + x] == 1)
        {
          --tolerance;

          if (tolerance <= 0)
          {
            break;
          }
        }
      }
      while (!TCOD_line_step_mt(&x, &y, &bresenham_data));

      if (tolerance <= 0)
      {
        return true;
      }
      return false;
    };

    while (!found_river_points)
    {
      ++tries;

      // Relax constraints with a high number of tries
      if (tries > island.structure.land_sites.size() * 2)
      {
        has_reached_second_limit = true;
      }
      else if (tries > island.structure.land_sites.size())
      {
        has_reached_first_limit = true;
      }

      source_site = island.structure.land_sites[land_indexes_dist(rng)];
      source_point = Point<int>(source_site->point.x * width, source_site->point.y * height);

      // Make sure that source point is on the upper part of the island
      if (!has_reached_first_limit && source_point.y > height / 2)
      {
        continue;
      }

      std::vector<int> bay_candidates;
      float max_mouth_source_distance = 0.f;
      int optimal_bay_index = -1;

      for (std::size_t i = 0; i < bays.size() - 1; ++i)
      {
        const auto& bay = bays[i];

        const int distance_x = std::abs(static_cast<int>(bay.x - source_point.x));
        const int distance_y = std::abs(static_cast<int>(bay.y - source_point.y));

        if (distance_x >= min_source_mouth_distance_x && distance_y >= min_source_mouth_distance_y && bay.y > source_point.y)
        {
          if (intersects_water(source_point, bay))
          {
            continue;
          }

          mouth_point = bays[i];
          found_river_points = true;
          break;
        }
        else if (has_reached_first_limit && bay.y > source_point.y && (distance_x >= min_source_mouth_distance_x || distance_y >= min_source_mouth_distance_y))
        {
          if (intersects_water(source_point, bay))
          {
            continue;
          }

          const float distance = m_distance(source_point, bay);

          if (distance > max_mouth_source_distance)
          {
            max_mouth_source_distance = distance;
            optimal_bay_index = i;
          }
        }
        else if (has_reached_second_limit)
        {
          if (intersects_water(source_point, bay))
          {
            continue;
          }

          const float distance = m_distance(source_point, bay);

          if (distance > max_mouth_source_distance)
          {
            max_mouth_source_distance = distance;
            optimal_bay_index = i;
          }
        }
      }

      if (optimal_bay_index != -1 && (has_reached_first_limit || has_reached_second_limit))
      {
        mouth_point = bays[optimal_bay_index];
        found_river_points = true;
        break;
      }
    }

    return {source_point, mouth_point};
  }

  std::vector<std::shared_ptr<RiverSegment>> TerrainGenerator::m_get_river_segments(const Point<int>& source, const Point<int>& mouth, std::vector<int>& tiles, const int width, const int seed)
  {
    std::vector<std::shared_ptr<RiverSegment>> river;

    const auto step = m_lua.get_variable<float>("river_bezier_step");
    const auto control1_delta_x = m_lua.get_variable<int>("river_control1_delta_x");
    const auto control1_delta_y = m_lua.get_variable<int>("river_control1_delta_y");
    const auto control2_delta_x = m_lua.get_variable<int>("river_control2_delta_x");
    const auto control2_delta_y = m_lua.get_variable<int>("river_control2_delta_y");

    const float control1_x = static_cast<float>(((source.x + mouth.x) / 3) + control1_delta_x);
    const float control1_y = static_cast<float>(((source.y + mouth.y) / 3) + control1_delta_y);

    const float control2_x = static_cast<float>((2 * (source.x + mouth.x) / 3) + control2_delta_x);
    const float control2_y = static_cast<float>((2 * (source.y + mouth.y) / 3) + control2_delta_y);

    Bezier::Bezier<3> cubic_bezier({ {static_cast<float>(source.x), static_cast<float>(source.y)}, {control1_x, control1_y}, {control2_x, control2_y}, {static_cast<float>(mouth.x), static_cast<float>(mouth.y)} });

    std::shared_ptr<RiverSegment> last_segment = nullptr;

    for (float i = 0.f; i < 1.f; i += step)
    {
      const auto point = cubic_bezier.valueAt(i);
      auto segment = std::make_shared<RiverSegment>();
      segment->point.x = std::round(point.x);
      segment->point.y = std::round(point.y);

      river.push_back(segment);
      const auto current_index = river.size() - 1;

      if (last_segment != nullptr)
      {
        // Add pointers between segments
        last_segment->next = river[current_index];
        river[current_index]->previous = last_segment;
      }

      last_segment = river[current_index];
    }

    assert(!river.empty() && "River curve was not generated");

    // Add connection from last segment to the mouth
    auto segment = std::make_shared<RiverSegment>();
    segment->point = mouth;
    river.push_back(segment);
    const auto current_index = river.size() - 1;
    last_segment->next = river[current_index];
    river[current_index]->previous = last_segment;

    return river;
  }

  void TerrainGenerator::m_create_meanders(std::vector<std::shared_ptr<RiverSegment>>& river, std::vector<int>& tiles, const int width, const int height)
  {
    std::vector<std::pair<Point<double>, Point<double>>> normals;

    for (auto& segment : river)
    {
      if (segment->next == nullptr || segment->previous == nullptr || segment->length == 0.0 || segment->previous->length == 0.0)
      {
        continue;
      }
      // Get curvature
      auto curvature = m_menger_curvature(segment->previous->point, segment->point, segment->next->point, segment->previous->length, segment->length, m_distance(segment->previous->point, segment->next->point));

      if (std::isnan(curvature))
      {
        continue;
      }

      if (curvature < 0)
      {
        curvature = std::min(curvature, -0.1);
        curvature = std::max(curvature, -0.8);
      }
      else
      {
        curvature = std::max(curvature, 0.1);
        curvature = std::min(curvature, 0.8);
      }
      /* std::cout << "CURVATURE: " << curvature << '\n'; */


      double normal_length = 40.0*curvature*-1.0;
      Point<int> normal_origin(static_cast<int>(segment->center.x), static_cast<int>(segment->center.y));
      Point<int> normal_destination(static_cast<int>(segment->center.x + normal_length*segment->normal.x), static_cast<int>(segment->center.y + normal_length*segment->normal.y));
      m_draw_line(normal_origin, normal_destination, 4, tiles, width, height);
    }
  }

  double TerrainGenerator::m_distance(const Point<double>& point_a, const Point<double>& point_b)
  {
    const double distance_x = point_a.x - point_b.x;
    const double distance_y = point_a.y - point_b.y;
    return sqrt(distance_x*distance_x + distance_y*distance_y);
  }

  float TerrainGenerator::m_distance(const Point<int>& point_a, const Point<int>& point_b)
  {
    const int distance_x = point_a.x - point_b.x;
    const int distance_y = point_a.y - point_b.y;
    return sqrt(distance_x*distance_x + distance_y*distance_y);
  }

  double TerrainGenerator::m_menger_curvature(const Point<int>& point_a, const Point<int>& point_b, const Point<int>& point_c, const double length_1, const double length_2, const double length_3)
  {
    return 2.0 * ((point_b.x - point_a.x)*(point_c.y - point_a.y) - (point_b.y - point_a.y)*(point_c.x - point_a.x)) / (length_1 * length_2 * length_3);
  }

  void TerrainGenerator::m_draw_point(const Point<int>& point, const int value, std::vector<int>& tiles, const int width)
  {
    tiles[point.y*width + point.x] = value;
  }

  void TerrainGenerator::m_draw_big_point(const Point<int>& point, const int value, std::vector<int>& tiles, const int width)
  {
    tiles[point.y*width + point.x] = value;
    tiles[point.left().y*width + point.left().x] = value;
    tiles[point.right().y*width + point.right().x] = value;
    tiles[point.bottom().y*width + point.bottom().x] = value;
    tiles[point.top().y*width + point.top().x] = value;
    /* tiles[point.top_left().y*width + point.top_left().x] = value; */
    /* tiles[point.top_right().y*width + point.top_right().x] = value; */
    /* tiles[point.bottom_left().y*width + point.bottom_left().x] = value; */
    /* tiles[point.bottom_right().y*width + point.bottom_right().x] = value; */
  }

  void TerrainGenerator::m_draw_line(const Point<int>& origin, const Point<int>& destination, const int value, std::vector<int>& tiles, const int width, const int height)
  {
    int x = origin.x;
    int y = origin.y;

    TCOD_bresenham_data_t bresenham_data;
    TCOD_line_init_mt(x, y, destination.x, destination.y, &bresenham_data);

    do
    {
      if (x < 0 || y < 0 || x > static_cast<int>(width) - 1 || y > static_cast<int>(height) - 1)
      {
        continue;
      }

      tiles[y*width + x] = value;
    }
    while (!TCOD_line_step_mt(&x, &y, &bresenham_data));
  }
}
