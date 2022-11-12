#include "./terrain_generator.hpp"

#include <cmath>
#include <utility>

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

    std::vector<int> tiles(width * height);

    m_generate_silhouette(tiles, width, height, seed);

    std::cout << "World silhouette generated\n";

    // Get a vector with remaining islands in crescent order (last element is the main island)
    const auto islands = m_adjust_and_get_islands(tiles, width, height);

    std::cout << "Islands adjusted\n";

    const auto& main_island = islands.back();

    m_generate_main_river(tiles, width, height, main_island);

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

    const auto tier_grass = m_lua.get_variable<float>("tier_grass");
    const auto tier_coast = m_lua.get_variable<float>("tier_coast");

    for (uint32_t j = 0; j < height; ++j)
    {
      for (uint32_t i = 0; i < width; ++i)
      {
        const float gradient = m_get_rectangle_gradient_value(i, j, width, height); 

        const float noise_value =  m_noise.GetNoise(static_cast<float>(i), static_cast<float>(j)) - gradient;

        int tile_value;

        if (noise_value > tier_grass)
        {
          tile_value = 2;
        }
        else if (noise_value > tier_coast)
        {
          tile_value = 3;
        }
        else
        {
          tile_value = 1;
        }

        tiles[j*width + i] = tile_value;
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

  void TerrainGenerator::m_generate_main_river(std::vector<int>& tiles, const uint32_t width, const uint32_t height, const std::vector<Point>& main_island)
  {
    // Search bay
    const auto coastline = m_get_coastline(tiles, width, height, main_island);

    // Get random point in mainland biased to the center and with a minimum distance to bay point

    // Build trajectory and meanders
    
    // Check that river doesn't join a lake or the sea
    // If it happens, search another random mainland point
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

    std::cout << "First point found: " << first_point.x << ' ' << first_point.y << '\n';

    std::queue<Point> point_queue;
    point_queue.push(first_point);

    while(!point_queue.empty())
    {
      const auto current_point = point_queue.front();
      point_queue.pop();

      coastline.push_back(current_point);
      mask[current_point.y*width + current_point.x] = 1;
      tiles[current_point.y*width + current_point.x] = 4;

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

    std::cout << "Costline identified: " << coastline.size() << '\n';

    return coastline;
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
}

