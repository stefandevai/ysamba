#include "./terrain_generator.hpp"

#include <cmath>
#include <utility>
#include <functional>
#include <libtcod.hpp>

// TEMP
#include <iostream>
// TEMP

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace dl
{
  FastNoiseLite TerrainGenerator::m_noise = FastNoiseLite{1337};
  LuaAPI TerrainGenerator::m_lua = LuaAPI{"generators/terrain.lua"};

  std::vector<int> TerrainGenerator::generate(const uint32_t width, const uint32_t height, const int seed)
  {
    m_lua.load("generators/terrain.lua");

    std::cout << "========================================\n";
    std::cout << "= STARTING DIALECTICS WORLD GENERATION =\n";
    std::cout << "========================================\n\n";

    std::vector<int> tiles(width * height);

    std::cout << "[*] Generating world silhouette\n";

    m_generate_silhouette(tiles, width, height, seed);

    std::cout << "[*] Adjusting islands\n";

    // Get a vector with remaining islands in crescent order (last element is the main island)
    const auto islands = m_adjust_and_get_islands(tiles, width, height);

    std::cout << "[*] Identifying coastline\n";

    auto coastline = m_get_coastline(tiles, width, height, islands.back());

    std::cout << "[*] Identifying bays\n";

    const auto bays = m_get_bays(coastline, islands.back(), width, height);

    std::cout << "[*] Building main island geometry\n";

    const auto main_island_geometry = m_get_island_geometry(width, height, islands.back());

    /* for (const auto& geometry_point : main_island_geometry) */
    /* { */
    /*   tiles[geometry_point.point.y*width + geometry_point.point.x] = 5; */
    /*   tiles[geometry_point.point.left().y*width + geometry_point.point.left().x] = 5; */
    /*   tiles[geometry_point.point.right().y*width + geometry_point.point.right().x] = 5; */
    /*   tiles[geometry_point.point.bottom().y*width + geometry_point.point.bottom().x] = 5; */
    /*   tiles[geometry_point.point.top().y*width + geometry_point.point.top().x] = 5; */
    /* } */

    for (const auto& bay : bays)
    {
      tiles[bay.y*width + bay.x] = 4;
      tiles[bay.left().y*width + bay.left().x] = 4;
      tiles[bay.right().y*width + bay.right().x] = 4;
      tiles[bay.bottom().y*width + bay.bottom().x] = 4;
      tiles[bay.top().y*width + bay.top().x] = 4;
    }

    /* m_generate_main_river(tiles, width, height, main_island_geometry); */

    std::cout << "[*] World generation finished\n\n";

    return tiles;
  }

  void TerrainGenerator::m_generate_silhouette(std::vector<int>& tiles, const uint32_t width, const uint32_t height, const int seed)
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

    for (uint32_t j = 0; j < height; ++j)
    {
      for (uint32_t i = 0; i < width; ++i)
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

    for (uint32_t j = 0; j < height && !has_flooded_ocean; ++j)
    {
      for (uint32_t i = 0; i < width && !has_flooded_ocean; ++i)
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
    for (uint32_t j = 0; j < height; ++j)
    {
      for (uint32_t i = 0; i < width; ++i)
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

  std::vector<std::vector<Point>> TerrainGenerator::m_adjust_and_get_islands(std::vector<int>& tiles, const int width, const int height)
  {
    auto all_islands = m_get_islands(tiles, width, height);

    const uint32_t number_of_islands_to_keep = m_lua.get_variable<uint32_t>("islands_to_keep");

    while (all_islands.size() > number_of_islands_to_keep)
    {
      const auto& current_island = all_islands.top();

      for (const auto& coord : current_island)
      {
        // Set island tiles to water to remove them
        tiles[coord.y*width + coord.x] = 1;
      }

      all_islands.pop();
    }

    // Keep remaining islands in a vector
    // As we ordered the priority queue in crescent order, the last
    // island will be the main island
    std::vector<std::vector<Point>> islands(all_islands.size());

    while(!all_islands.empty())
    {
      islands.push_back(std::move(const_cast<std::vector<Point>&>(all_islands.top())));
      all_islands.pop();
    }

    return islands;
  }

  island_queue TerrainGenerator::m_get_islands(const std::vector<int>& tiles, const uint32_t width, const uint32_t height)
  {
    std::vector<int> mask(width * height, 0);
    island_queue islands{};

    for (uint32_t j = 0; j < height; ++j)
    {
      for (uint32_t i = 0; i < width; ++i)
      {
        const auto tile_value = tiles[j*width + i];
        const auto mask_value = mask[j*width + i];

        // Not water and not masked
        if (tile_value != 1 && mask_value == 0)
        {
          const auto island = m_get_island(tiles, mask, i, j, width, height);
          islands.push(island);
        }
      }
    }

    return islands;
  }

  std::vector<Point> TerrainGenerator::m_get_island(const std::vector<int>& tiles, std::vector<int>& mask, const uint32_t x, const uint32_t y, const uint32_t width, const uint32_t height)
  {
    assert(m_valid_coord(x, y, width, height));

    std::vector<Point> island{Point(x, y)};
    std::queue<Point> coord_queue;

    coord_queue.push(Point(x, y));

    while (!coord_queue.empty())
    {
      auto coord = coord_queue.front();

      const uint32_t x0 = coord.x;
      const uint32_t y0 = coord.y;

      mask[y0*width + x0] = 1;

      coord_queue.pop();

      // Top coord
      const auto yt = y0 + 1;
      const auto top_coord = yt*width + x0;
      if (m_valid_coord(x0, yt, width, height) && tiles[top_coord] != 1 && mask[top_coord] == 0)
      {
        mask[top_coord] = 1;
        island.push_back(Point(x0, yt));
        coord_queue.push(Point(x0, yt));
      }

      // Bottom coord
      const auto yb = y0 - 1;
      const auto bottom_coord = yb*width + x0;
      if (m_valid_coord(x0, yb, width, height) && tiles[bottom_coord] != 1 && mask[bottom_coord] == 0)
      {
        mask[bottom_coord] = 1;
        island.push_back(Point(x0, yb));
        coord_queue.push(Point(x0, yb));
      }

      // Left coord
      const auto xl = x0 - 1;
      const auto left_coord = y0*width + xl;
      if (m_valid_coord(xl, y0, width, height) && tiles[left_coord] != 1 && mask[left_coord] == 0)
      {
        mask[left_coord] = 1;
        island.push_back(Point(xl, y0));
        coord_queue.push(Point(xl, y0));
      }

      // Right coord
      const auto xr = x0 + 1;
      const auto right_coord = y0*width + xr;
      if (m_valid_coord(xr, y0, width, height) && tiles[right_coord] != 1 && mask[right_coord] == 0)
      {
        mask[right_coord] = 1;
        island.push_back(Point(xr, y0));
        coord_queue.push(Point(xr, y0));
      }
    }

    return island;
  }

  bool TerrainGenerator::m_valid_coord(const uint32_t x, const uint32_t y, const uint32_t width, const uint32_t height)
  {
    return !(x < 0 || y < 0 || x >= width || y >= height);
  }

  bool TerrainGenerator::m_valid_point(const Point& point, const uint32_t width, const uint32_t height)
  {
    return !(point.x < 0 || point.y < 0 || point.x >= width || point.y >= height);
  }

  /* void TerrainGenerator::m_generate_main_river(std::vector<int>& tiles, const uint32_t width, const uint32_t height, const std::vector<GeometryPoint>& geometry) */
  /* { */
  /*   // Search bay */

  /*   // Get random point in mainland biased to the center and with a minimum distance to bay point */

  /*   // Build trajectory and meanders */
    
  /*   // Check that river doesn't join a lake or the sea */
  /*   // If it happens, search another random mainland point */
  /* } */

  bool TerrainGenerator::m_boders_water(const GeometryPoint& geometry_point)
  {
    return (geometry_point.top_left == nullptr || geometry_point.top == nullptr || geometry_point.top_right == nullptr ||
            geometry_point.left == nullptr || geometry_point.right == nullptr ||
            geometry_point.bottom_left == nullptr || geometry_point.bottom == nullptr || geometry_point.bottom_right == nullptr);
  }

  std::vector<Point> TerrainGenerator::m_get_coastline(std::vector<int>& tiles, const uint32_t width, const uint32_t height, const std::vector<Point>& main_island)
  {
    std::vector<Point> coastline;
    std::vector<uint32_t> mask(width * height, 0);

    const auto first_point = m_get_first_coastline_point(tiles, width, height, main_island);

    // TODO: Handle not found
    if (first_point.x == 0 && first_point.y == 0)
    {
      return coastline;
    }

    std::queue<Point> point_queue;
    point_queue.push(first_point);

    while(!point_queue.empty())
    {
      const auto current_point = point_queue.front();
      point_queue.pop();

      coastline.push_back(current_point);
      mask[current_point.y*width + current_point.x] = 1;
      /* tiles[current_point.y*width + current_point.x] = 4; */

      const auto left_point = current_point.left();
      const auto right_point = current_point.right();
      const auto bottom_point = current_point.bottom();
      const auto top_point = current_point.top();

      auto manipulate_point = [width, height, &mask, &point_queue, &tiles](const Point& point)
      {
        if (m_valid_point(point, width, height) && m_is_coast_point(point, width, height, tiles) && mask[point.y*width + point.x] == 0)
        {
          point_queue.push(point);
        }
      };

      manipulate_point(left_point);
      manipulate_point(right_point);
      manipulate_point(bottom_point);
      manipulate_point(top_point);
    }

    return coastline;
  }

  std::vector<GeometryPoint> TerrainGenerator::m_get_island_geometry(const uint32_t width, const uint32_t height, const std::vector<Point>& island)
  {
    assert(island.size() > 0 && "Main island size is empty");

    std::vector<Point> coastline{};
    const auto segment_width_h = m_lua.get_variable<uint32_t>("island_polygon_segment_h");
    const auto segment_width_v = m_lua.get_variable<uint32_t>("island_polygon_segment_v");

    std::vector<int> island_mask(width * height, 0);
    Point top_left = island[0];
    Point bottom_right = island[0];

    for (const auto& point : island)
    {
      // Create mask
      island_mask[point.y*width + point.x] = 1;

      // Get top left and bottom right points
      top_left.x = std::min(top_left.x, point.x);
      top_left.y = std::min(top_left.y, point.y);
      bottom_right.x = std::max(bottom_right.x, point.x);
      bottom_right.y = std::max(bottom_right.y, point.y);
    }

    bool found_first_point = false;
    Point first_point(0, 0);

    // Horizontal top line
    for (uint32_t j = top_left.y; j < bottom_right.y; j += segment_width_v)
    {
      for (uint32_t i = top_left.x; i < bottom_right.x; i += segment_width_h)
      {
        auto current_point = Point(i, j);

        if (island_mask[current_point.y*width + current_point.x] == 0)
        /* if (!m_contains_island_area(current_point, segment_width_h, segment_width_v, width, height, island_mask)) */
        {
          continue;
        }

        first_point.x = i;
        first_point.y = j;
        found_first_point = true;
        break;
      }

      if (found_first_point)
      {
        break;
      }
    }

    std::vector<GeometryPoint> geometry;
    std::queue<Point> point_queue;
    point_queue.push(first_point);

    while(!point_queue.empty())
    {
      const auto current_point = point_queue.front();
      const Point top_left(current_point.x - segment_width_h, current_point.y - segment_width_v);
      const Point top(current_point.x, current_point.y - segment_width_v);
      const Point top_right(current_point.x + segment_width_h, current_point.y - segment_width_v);
      const Point left(current_point.x - segment_width_h, current_point.y);
      const Point right(current_point.x + segment_width_h, current_point.y);
      const Point bottom_left(current_point.x - segment_width_h, current_point.y + segment_width_v);
      const Point bottom(current_point.x, current_point.y + segment_width_v);
      const Point bottom_right(current_point.x + segment_width_h, current_point.y + segment_width_v);

      point_queue.pop();

      auto check_water = [width, height, &island_mask, &point_queue, &geometry](const Point& point)
      {
        const auto index = point.y*width + point.x;

        if (m_valid_point(point, width, height) && island_mask[index] != 0)
        {
          if (island_mask[index] == 1)
          {
            island_mask[index] = 2;
            point_queue.push(point);
            geometry.push_back(GeometryPoint(point));
          }
        }
      };

      check_water(top_left);
      check_water(top);
      check_water(top_right);
      check_water(left);
      check_water(right);
      check_water(bottom_left);
      check_water(bottom);
      check_water(bottom_right);
    }

    // Add vertice data
    for (auto& geometry_point : geometry)
    {
      const Point top_left(geometry_point.point.x - segment_width_h, geometry_point.point.y - segment_width_v);
      const Point top(geometry_point.point.x, geometry_point.point.y - segment_width_v);
      const Point top_right(geometry_point.point.x + segment_width_h, geometry_point.point.y - segment_width_v);
      const Point left(geometry_point.point.x - segment_width_h, geometry_point.point.y);
      const Point right(geometry_point.point.x + segment_width_h, geometry_point.point.y);
      const Point bottom_left(geometry_point.point.x - segment_width_h, geometry_point.point.y + segment_width_v);
      const Point bottom(geometry_point.point.x, geometry_point.point.y + segment_width_v);
      const Point bottom_right(geometry_point.point.x + segment_width_h, geometry_point.point.y + segment_width_v);

      for (auto& geometry_point_aux : geometry)
      {
        if (geometry_point_aux.point == top_left)
        {
          geometry_point.top_left = &geometry_point_aux;
        }
        else if (geometry_point_aux.point == top)
        {
          geometry_point.top = &geometry_point_aux;
        }
        else if (geometry_point_aux.point == top_right)
        {
          geometry_point.top_right = &geometry_point_aux;
        }
        else if (geometry_point_aux.point == left)
        {
          geometry_point.left = &geometry_point_aux;
        }
        else if (geometry_point_aux.point == right)
        {
          geometry_point.right = &geometry_point_aux;
        }
        else if (geometry_point_aux.point == bottom_left)
        {
          geometry_point.bottom_left = &geometry_point_aux;
        }
        else if (geometry_point_aux.point == bottom)
        {
          geometry_point.bottom = &geometry_point_aux;
        }
        else if (geometry_point_aux.point == bottom_right)
        {
          geometry_point.bottom_right = &geometry_point_aux;
        }
      }
    }

    return geometry;
  }

  bool TerrainGenerator::m_contains_island_area(const Point& point, const float segment_width_h, const float segment_width_v, const uint32_t width, const uint32_t height, const std::vector<int>& island_mask)
  {
    const auto top_left_x = std::max(static_cast<int>(point.x - std::round(segment_width_h)), 0);
    const auto top_left_y = std::max(static_cast<int>(point.y - std::round(segment_width_v)), 0);
    const auto bottom_right_x = std::min(static_cast<int>(point.x + std::round(segment_width_h)), static_cast<int>(width));
    const auto bottom_right_y = std::min(static_cast<int>(point.y + std::round(segment_width_v)), static_cast<int>(height));
    
    for (auto i = top_left_x; i < bottom_right_x; ++i)
    {
      for (auto j = top_left_y; j < bottom_right_y; ++j)
      {
        if (island_mask[j*width + i] != 0)
        {
          return true;
        }
      }
    }

    return false;
  }

  void TerrainGenerator::m_flood_fill(const int value, const uint32_t x, const uint32_t y, std::vector<int>& tiles, const uint32_t width, const uint32_t height)
  {
    assert(m_valid_coord(x, y, width, height));

    const int original_value = tiles[y*width + x];
    std::queue<Point> coord_queue;
    std::vector<int> mask(width * height, 0);

    coord_queue.push(Point(x, y));
    tiles[y*width + x] = value;

    while (!coord_queue.empty())
    {
      auto coord = coord_queue.front();
      const uint32_t x0 = coord.x;
      const uint32_t y0 = coord.y;

      coord_queue.pop();

      // Top coord
      const auto yt = y0 + 1;
      const auto top_coord = yt*width + x0;
      if (m_valid_coord(x0, yt, width, height) && tiles[top_coord] == original_value && mask[top_coord] == 0)
      {
        mask[top_coord] = 1;
        tiles[top_coord] = value;
        coord_queue.push(Point(x0, yt));
      }

      // Bottom coord
      const auto yb = y0 - 1;
      const auto bottom_coord = yb*width + x0;
      if (m_valid_coord(x0, yb, width, height) && tiles[bottom_coord] == original_value && mask[bottom_coord] == 0)
      {
        mask[bottom_coord] = 1;
        tiles[bottom_coord] = value;
        coord_queue.push(Point(x0, yb));
      }

      // Left coord
      const auto xl = x0 - 1;
      const auto left_coord = y0*width + xl;
      if (m_valid_coord(xl, y0, width, height) && tiles[left_coord] == original_value && mask[left_coord] == 0)
      {
        mask[left_coord] = 1;
        tiles[left_coord] = value;
        coord_queue.push(Point(xl, y0));
      }

      // Right coord
      const auto xr = x0 + 1;
      const auto right_coord = y0*width + xr;
      if (m_valid_coord(xr, y0, width, height) && tiles[right_coord] == original_value && mask[right_coord] == 0)
      {
        mask[right_coord] = 1;
        tiles[right_coord] = value;
        coord_queue.push(Point(xr, y0));
      }
    }
  }

  Point TerrainGenerator::m_get_first_coastline_point(const std::vector<int>& tiles, const uint32_t width, const uint32_t height, const std::vector<Point>& main_island)
  {
    for (const auto& point : main_island)
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
    return Point(width, height);
  }

  int TerrainGenerator::m_get_point_value(const Point& point, const uint32_t width, const std::vector<int>& tiles)
  {
    return tiles[point.y*width + point.x];
  }

  bool TerrainGenerator::m_is_coast_point(const Point& point, const uint32_t width, const uint32_t height, const std::vector<int>& tiles)
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

  bool TerrainGenerator::m_is_water(const Point& point, const int width, const uint32_t height, const std::vector<int>& tiles)
  {
    return m_valid_point(point, width, height) && m_get_point_value(point, width, tiles) == 1;
  }

  std::vector<Point> TerrainGenerator::m_get_bays(std::vector<Point>& coastline, const std::vector<Point>& island, const uint32_t width, const uint32_t height)
  {
    std::vector<Point> bays;

    const auto minimum_area = m_lua.get_variable<int>("coast_min_area");
    const auto points_min_distance = m_lua.get_variable<int>("coast_points_min_distance");
    const auto points_max_distance = m_lua.get_variable<int>("coast_points_max_distance");

    std::vector<std::pair<Point, Point>> candidates;
    Point* coast_point_a = nullptr;
    Point* coast_point_b = nullptr;

    std::vector<int> main_insland_mask(width*height, 1);
    std::vector<int> area_mask(width*height, 0);

    for (const auto& point : island)
    {
      main_insland_mask[point.y * width + point.x] = 2;
    }

    for (uint32_t n = 0; n < coastline.size(); ++n)
    {
      bool found_candidate = false;

      for (uint32_t m = 0; m < coastline.size(); ++m)
      {
        if (n == m)
        {
          continue;
        }

        auto& point_a = coastline[n];
        auto& point_b = coastline[m];

        const int distance_x = point_a.x - point_b.x;
        const int distance_y = point_a.y - point_b.y;
        const float distance = sqrt(distance_x*distance_x + distance_y*distance_y);

        if (distance < points_min_distance || distance > points_max_distance)
        {
          continue;
        }

        if (!m_has_land_intersection(point_a, point_b, main_insland_mask, width))
        {
          found_candidate = true;
          coast_point_a = &point_a;
          coast_point_b = &point_b;
        }
        else if (found_candidate && coast_point_a != nullptr && coast_point_b != nullptr)
        {
          const auto bay_data = m_get_bay_data(*coast_point_a, *coast_point_b, main_insland_mask, width, height, minimum_area, area_mask);

          if (bay_data.area >= minimum_area)
          {
            bays.push_back(bay_data.point);
            /* candidates.push_back({ *coast_point_a, *coast_point_b }); */
          }

          found_candidate = false;

          /* if ((m - 1) > n) */
          /* { */
          /*   n = m - 1; */
          /* } */
          break;
        }

        /* if (candidates.size() > 0) */
        /* { */
        /*   break; */
        /* } */
      }
    }

    /* int count = 0; */

    /* for (const auto& candidate : candidates) */
    /* { */
    /*   tiles[candidate.first.y*width + candidate.first.x] = 5; */
    /*   tiles[candidate.first.left().y*width + candidate.first.left().x] = 5; */
    /*   tiles[candidate.first.right().y*width + candidate.first.right().x] = 5; */
    /*   tiles[candidate.first.bottom().y*width + candidate.first.bottom().x] = 5; */
    /*   tiles[candidate.first.top().y*width + candidate.first.top().x] = 5; */

    /*   tiles[candidate.second.y*width + candidate.second.x] = 5; */
    /*   tiles[candidate.second.left().y*width + candidate.second.left().x] = 5; */
    /*   tiles[candidate.second.right().y*width + candidate.second.right().x] = 5; */
    /*   tiles[candidate.second.bottom().y*width + candidate.second.bottom().x] = 5; */
    /*   tiles[candidate.second.top().y*width + candidate.second.top().x] = 5; */

    /*   int x = candidate.first.x; */
    /*   int y = candidate.first.y; */

    /*   TCOD_bresenham_data_t bresenham_data; */
    /*   TCOD_line_init_mt(x, y, candidate.second.x, candidate.second.y, &bresenham_data); */

    /*   do */
    /*   { */
    /*     tiles[y*width + x] = 5; */
    /*   } */
    /*   while (!TCOD_line_step_mt(&x, &y, &bresenham_data)); */

    /*   count++; */
    /* } */

    return bays;
  }

  bool TerrainGenerator::m_has_land_intersection(const Point& point_a, const Point& point_b, std::vector<int>& tiles, const uint32_t width)
  {
    const uint32_t middle_x = (point_a.x + point_b.x)/2;
    const uint32_t middle_y = (point_a.y + point_b.y)/2;

    if (tiles[middle_y*width + middle_x] != 1)
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

      if (tiles[y*width + x] != 1)
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

  BayData TerrainGenerator::m_get_bay_data(const Point& point_a, const Point& point_b, std::vector<int>& tiles, const uint32_t width, const uint32_t height, const int minimum, std::vector<int>& mask)
  {
    Point bay_point(point_a.x, point_b.y);

    {
      int x = point_a.x;
      int y = point_a.y;

      TCOD_bresenham_data_t bresenham_data;
      TCOD_line_init_mt(x, y, point_b.x, point_b.y, &bresenham_data);

      do
      {
        if(mask[y*width + x] == 1)
        {
          return BayData(0, bay_point);
        }

        mask[y*width + x] = 1;
      }
      while (!TCOD_line_step_mt(&x, &y, &bresenham_data));
    }

    int start_x, start_y;
    const uint32_t middle_x = (point_a.x + point_b.x)/2;
    const uint32_t middle_y = (point_a.y + point_b.y)/2;

    // Find perpendicular line
    {
      const int delta_x = point_b.x - point_a.x;
      const int delta_y = point_b.y - point_a.y;

      int steps_direction_1 = 0;
      int steps_direction_2 = 0;

      Point candidate_a(middle_x, middle_y);
      Point candidate_b(middle_x, middle_y);

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

          if (py >= 0 && static_cast<uint32_t>(py) < height && tiles[py*width + px] != 1)
          {
            if (mask[py*width + px] != 0)
            {
              return BayData(0, bay_point);
            }

            // Get previous point
            candidate_a.x = px - 1;
            candidate_a.y = static_cast<int>(std::round(perpendicular_y(px - 1)));
            break;
          }
          else if (py < 2 || static_cast<uint32_t>(py) >= height)
          {
            steps_direction_1 = height + 1;
            break;
          }
        }
        for (auto px = middle_x - 1; px > 0; --px)
        {
          ++steps_direction_2;
          const auto py = static_cast<int>(std::round(perpendicular_y(px)));

          if (py >= 0 && static_cast<uint32_t>(py) < height && tiles[py*width + px] != 1)
          {
            if (mask[py*width + px] != 0)
            {
              return BayData(0, bay_point);
            }

            candidate_b.x = px + 1;
            candidate_b.y = static_cast<int>(std::round(perpendicular_y(px + 1)));
            break;
          }
          else if (py < 2 || static_cast<uint32_t>(py) >= height || px < 2)
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
    std::queue<Point> coord_queue;

    coord_queue.push(Point(start_x, start_y));
    mask[start_y*width + start_x] = 1;
    area++;

    while (!coord_queue.empty())
    {
      auto coord = coord_queue.front();
      const uint32_t x0 = coord.x;
      const uint32_t y0 = coord.y;

      coord_queue.pop();

      // Top coord
      const auto yt = y0 + 1;
      const auto top_coord = yt*width + x0;
      if (m_valid_coord(x0, yt, width, height) && tiles[top_coord] == 1 && mask[top_coord] == 0)
      {
        mask[top_coord] = 1;
        coord_queue.push(Point(x0, yt));
        area++;
      }

      // Bottom coord
      const auto yb = y0 - 1;
      const auto bottom_coord = yb*width + x0;
      if (m_valid_coord(x0, yb, width, height) && tiles[bottom_coord] == 1 && mask[bottom_coord] == 0)
      {
        mask[bottom_coord] = 1;
        coord_queue.push(Point(x0, yb));
        area++;
      }

      // Left coord
      const auto xl = x0 - 1;
      const auto left_coord = y0*width + xl;
      if (m_valid_coord(xl, y0, width, height) && tiles[left_coord] == 1 && mask[left_coord] == 0)
      {
        mask[left_coord] = 1;
        coord_queue.push(Point(xl, y0));
        area++;
      }

      // Right coord
      const auto xr = x0 + 1;
      const auto right_coord = y0*width + xr;
      if (m_valid_coord(xr, y0, width, height) && tiles[right_coord] == 1 && mask[right_coord] == 0)
      {
        mask[right_coord] = 1;
        coord_queue.push(Point(xr, y0));
        area++;
      }

      if (area >= minimum)
      {
        break;
      }
    }

    return BayData(area, bay_point);
  }
}

