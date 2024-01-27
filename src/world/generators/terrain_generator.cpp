#include "./terrain_generator.hpp"

#include <spdlog/spdlog.h>

#include <chrono>
#include <cmath>
#include <functional>
#include <libtcod.hpp>
#include <utility>

#include "./bay_data.hpp"
#include "./lib/bezier.hpp"
#include "./lib/fast_noise_lite.hpp"
#include "./lib/gal/fortune_algorithm.hpp"
#include "./lib/poisson_disk_sampling.hpp"
#include "./terrain_type.hpp"
#include "core/random.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace dl
{
Tilemap TerrainGenerator::generate(const int seed)
{
  // TEMP
  m_json.load("./data/scripts/generators/terrain.json");
  // TEMP

  spdlog::info("=============================");
  spdlog::info("= STARTING WORLD GENERATION =");
  spdlog::info("=============================\n");
  spdlog::info("SEED: {}", seed);
  spdlog::info("WIDTH: {}", m_width);
  spdlog::info("HEIGHT: {}\n", m_height);

  std::vector<int> tiles(m_width * m_height);
  Tilemap tilemap{tiles, m_width, m_height};

  auto start = std::chrono::high_resolution_clock::now();

  spdlog::info("Generating world silhouette...");
  spdlog::info("");

  m_generate_silhouette(tilemap.tiles, seed);

  spdlog::info("Adjusting islands...");

  // Get a vector with remaining islands in crescent order (last element is the main island)
  auto islands = m_get_islands(tilemap.tiles);
  auto& main_island = islands.back();

  spdlog::info("Identifying coastline...");

  auto coastline = m_get_coastline(tilemap.tiles, main_island);

  spdlog::info("Identifying bays...");

  auto bays = m_get_bays(coastline, main_island);

  spdlog::info("Building main island geometry...");

  m_build_island_structure(main_island);

  spdlog::info("Generating main river...");

  m_generate_main_river(main_island, bays, tilemap.tiles, seed);

  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

  const auto draw_features = m_json.object["draw_features"].get<bool>();

  if (draw_features)
  {
    // Draw structure
    const auto draw_structure = m_json.object["draw_structure"].get<bool>();
    if (draw_structure)
    {
      for (auto site : main_island.structure.diagram.get_sites())
      {
        m_draw_big_point(site.point.convert(m_width, m_height), TerrainType::Yellow, tiles);
      }
      /* for (const auto& site : main_island.structure.sites) */
      /* for (const auto index : main_island.structure.coast_indexes) */
      /* { */
      /* m_draw_big_point(main_island.structure.sites[index].center, TerrainType::Yellow, tiles, m_width); */

      /* for (const auto& edge : main_island.structure.sites[index].edges) */
      /* { */
      /*   m_draw_line(edge.first, edge.second, TerrainType::Yellow, tiles, m_width, m_height); */
      /* } */

      /* m_draw_big_point(site.center, 5, tiles, m_width); */

      /* for (const auto& edge : site.edges) */
      /* { */
      /*   m_draw_line(edge.first, edge.second, TerrainType::Yellow, tiles, m_width, m_height); */
      /* } */
      /* } */
    }

    // Draw coastline
    const auto draw_coastline = m_json.object["draw_coastline"].get<bool>();
    if (draw_coastline)
    {
      for (const auto& coastline_point : coastline)
      {
        m_draw_point(coastline_point, TerrainType::Red, tilemap.tiles);
      }
    }

    // Draw bays
    const auto draw_bays = m_json.object["draw_bays"].get<bool>();
    if (draw_bays)
    {
      for (const auto& bay : bays)
      {
        m_draw_big_point(bay, TerrainType::None, tilemap.tiles);
      }
    }
  }

  spdlog::info("World generation finished! It took {} milliseconds", duration.count());

  return tilemap;
}

void TerrainGenerator::m_generate_silhouette(std::vector<int>& tiles, const int seed)
{
  const auto simplex_freq = m_json.object["simplex_freq"].get<float>();
  const auto simplex_octaves = m_json.object["simplex_octaves"].get<int>();
  const auto simplex_lacunarity = m_json.object["simplex_lacunarity"].get<float>();
  const auto simplex_gain = m_json.object["simplex_gain"].get<float>();
  const auto simplex_weighted_strength = m_json.object["simplex_weighted_strength"].get<float>();

  auto noise = FastNoiseLite{seed};
  noise.SetSeed(seed);
  noise.SetNoiseType(FastNoiseLite::NoiseType::NoiseType_OpenSimplex2S);
  noise.SetRotationType3D(FastNoiseLite::RotationType3D_ImproveXYPlanes);
  noise.SetFrequency(simplex_freq);
  noise.SetFractalType(FastNoiseLite::FractalType::FractalType_FBm);
  noise.SetFractalOctaves(simplex_octaves);
  noise.SetFractalLacunarity(simplex_lacunarity);
  noise.SetFractalGain(simplex_gain);
  noise.SetFractalWeightedStrength(simplex_weighted_strength);

  const auto tier_land = m_json.object["tier_land"].get<float>();

  for (int j = 0; j < m_height; ++j)
  {
    for (int i = 0; i < m_width; ++i)
    {
      const float gradient = m_get_rectangle_gradient_value(i, j);

      const float noise_value = noise.GetNoise(static_cast<float>(i), static_cast<float>(j)) - gradient;

      int tile_value;

      if (noise_value > tier_land)
      {
        tile_value = TerrainType::Land;
      }
      else
      {
        tile_value = TerrainType::None;
      }

      tiles[j * m_width + i] = tile_value;
    }
  }

  // Remove lakes
  bool has_flooded_ocean = false;

  for (int j = 0; j < m_height && !has_flooded_ocean; ++j)
  {
    for (int i = 0; i < m_width && !has_flooded_ocean; ++i)
    {
      if (i != 0 && j != 0)
      {
        continue;
      }

      const auto value = tiles[j * m_width + i];

      if (value == 0)
      {
        if (j == 0 || i == 0)
        {
          m_flood_fill(1, i, j, tiles);
          has_flooded_ocean = true;
        }
      }
    }
  }

  // Replace inland water with grass
  for (int j = 0; j < m_height; ++j)
  {
    for (int i = 0; i < m_width; ++i)
    {
      if (tiles[j * m_width + i] == 0)
      {
        tiles[j * m_width + i] = 2;
      }
    }
  }
}

float TerrainGenerator::m_get_rectangle_gradient_value(const int x, const int y)
{
  auto distance_to_edge = std::min(abs(x - m_width), x);
  distance_to_edge = std::min(distance_to_edge, abs(y - m_height));
  distance_to_edge = std::min(distance_to_edge, y) * 2;
  return 1.f - static_cast<float>(distance_to_edge) / (m_width / 2.0f);
}

std::vector<IslandData> TerrainGenerator::m_get_islands(std::vector<int>& tiles)
{
  auto all_islands = m_get_island_queue(tiles);

  const auto number_of_islands_to_keep = m_json.object["islands_to_keep"].get<std::uint32_t>();

  while (all_islands.size() > number_of_islands_to_keep)
  {
    const auto& current_island = all_islands.top();

    for (const auto& coord : current_island.points)
    {
      // Set island tiles to water to remove them
      tiles[coord.y * m_width + coord.x] = 1;
    }

    all_islands.pop();
  }

  // Keep remaining islands in a vector
  // As we ordered the priority queue in crescent order, the last
  // island will be the main island
  std::vector<IslandData> islands(all_islands.size());

  while (!all_islands.empty())
  {
    islands.push_back(std::move(const_cast<IslandData&>(all_islands.top())));
    all_islands.pop();
  }

  return islands;
}

IslandQueue TerrainGenerator::m_get_island_queue(const std::vector<int>& tiles)
{
  std::vector<int> mask(m_width * m_height, TerrainType::Water);
  IslandQueue islands{};

  for (int j = 0; j < m_height; ++j)
  {
    for (int i = 0; i < m_width; ++i)
    {
      const auto tile_value = tiles[j * m_width + i];
      const auto mask_value = mask[j * m_width + i];

      // Not water and not masked
      if (tile_value != TerrainType::Water && mask_value == TerrainType::Water)
      {
        const auto island_data = m_get_island(tiles, mask, i, j);
        islands.push(island_data);
      }
    }
  }

  return islands;
}

IslandData TerrainGenerator::m_get_island(const std::vector<int>& tiles,
                                          std::vector<int>& mask,
                                          const int x,
                                          const int y)
{
  assert(m_valid_coord(x, y) && "Island coordinates are not valid");

  IslandData island_data;
  island_data.mask.resize(m_width * m_height, TerrainType::Water);

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

    island_data.mask[y0 * m_width + x0] = TerrainType::Land;
    island_data.points.push_back(Point<int>(x0, y0));
    island_data.top_left.x = std::min(x0, island_data.top_left.x);
    island_data.top_left.y = std::min(y0, island_data.top_left.y);
    island_data.bottom_right.x = std::max(x0, island_data.bottom_right.x);
    island_data.bottom_right.y = std::max(y0, island_data.bottom_right.y);

    coord_queue.pop();

    // Top coord
    const auto yt = y0 + 1;
    const auto top_coord = yt * m_width + x0;
    if (m_valid_coord(x0, yt) && tiles[top_coord] != TerrainType::Water && mask[top_coord] == TerrainType::Water)
    {
      mask[top_coord] = TerrainType::Land;
      coord_queue.push(Point<int>(x0, yt));
    }

    // Bottom coord
    const auto yb = y0 - 1;
    const auto bottom_coord = yb * m_width + x0;
    if (m_valid_coord(x0, yb) && tiles[bottom_coord] != TerrainType::Water && mask[bottom_coord] == TerrainType::Water)
    {
      mask[bottom_coord] = TerrainType::Land;
      coord_queue.push(Point<int>(x0, yb));
    }

    // Left coord
    const auto xl = x0 - 1;
    const auto left_coord = y0 * m_width + xl;
    if (m_valid_coord(xl, y0) && tiles[left_coord] != TerrainType::Water && mask[left_coord] == TerrainType::Water)
    {
      mask[left_coord] = TerrainType::Land;
      coord_queue.push(Point<int>(xl, y0));
    }

    // Right coord
    const auto xr = x0 + 1;
    const auto right_coord = y0 * m_width + xr;
    if (m_valid_coord(xr, y0) && tiles[right_coord] != TerrainType::Water && mask[right_coord] == TerrainType::Water)
    {
      mask[right_coord] = TerrainType::Land;
      coord_queue.push(Point<int>(xr, y0));
    }
  }

  return island_data;
}

bool TerrainGenerator::m_valid_coord(const int x, const int y)
{
  return !(x < 0 || y < 0 || x >= m_width || y >= m_height);
}

bool TerrainGenerator::m_valid_point(const Point<int>& point)
{
  return !(point.x < 0 || point.y < 0 || point.x >= m_width || point.y >= m_height);
}

std::vector<Point<int>> TerrainGenerator::m_get_coastline(const std::vector<int>& tiles, const IslandData& island)
{
  std::vector<Point<int>> coastline;
  std::vector<int> mask(m_width * m_height, TerrainType::Water);
  const std::size_t mask_size = mask.size();

  const auto first_point = m_get_first_coastline_point(tiles, island);
  mask[first_point.y * m_width + first_point.x] = TerrainType::Land;

  // TODO: Handle not found
  if (first_point.x == 0 && first_point.y == 0)
  {
    return coastline;
  }

  std::queue<Point<int>> point_queue;
  point_queue.push(first_point);

  while (!point_queue.empty())
  {
    const auto current_point = point_queue.front();
    point_queue.pop();

    coastline.push_back(current_point);
    mask[current_point.y * m_width + current_point.x] = TerrainType::Land;

    const auto left_point = current_point.left();
    const auto right_point = current_point.right();
    const auto bottom_point = current_point.bottom();
    const auto top_point = current_point.top();

    auto manipulate_point = [this, mask_size, &mask, &point_queue, &tiles](const Point<int>& point)
    {
      if (m_valid_point(point) && m_is_coast_point(point, tiles) &&
          mask[point.y * m_width + point.x] == TerrainType::Water)
      {
        int neighbours = 0;

        const std::size_t top_left_coord = (point.y - 1) * m_width + point.x - 1;
        const std::size_t top_coord = (point.y - 1) * m_width + point.x;
        const std::size_t top_right_coord = (point.y - 1) * m_width + point.x + 1;
        const std::size_t left_coord = point.y * m_width + point.x - 1;
        const std::size_t right_coord = point.y * m_width + point.x + 1;
        const std::size_t bottom_left_coord = (point.y + 1) * m_width + point.x - 1;
        const std::size_t bottom_coord = (point.y + 1) * m_width + point.x;
        const std::size_t bottom_right_coord = (point.y + 1) * m_width + point.x + 1;

        // Top left
        if (top_left_coord < mask_size && mask[top_left_coord] == TerrainType::Land)
        {
          ++neighbours;
        }
        if (top_coord < mask_size && mask[top_coord] == TerrainType::Land)
        {
          ++neighbours;
        }
        if (top_right_coord < mask_size && mask[top_right_coord] == TerrainType::Land)
        {
          ++neighbours;
        }
        if (left_coord < mask_size && mask[left_coord] == TerrainType::Land)
        {
          ++neighbours;
        }
        if (right_coord < mask_size && mask[right_coord] == TerrainType::Land)
        {
          ++neighbours;
        }
        if (bottom_left_coord < mask_size && mask[bottom_left_coord] == TerrainType::Land)
        {
          ++neighbours;
        }
        if (bottom_coord < mask_size && mask[bottom_coord] == TerrainType::Land)
        {
          ++neighbours;
        }
        if (bottom_right_coord < mask_size && mask[bottom_right_coord] == TerrainType::Land)
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

bool TerrainGenerator::m_contains_island_area(const Point<int>& point,
                                              const float segment_m_width_h,
                                              const float segment_m_width_v,
                                              const std::vector<int>& island_mask)
{
  const auto top_left_x = std::max(static_cast<int>(point.x - std::round(segment_m_width_h)), 0);
  const auto top_left_y = std::max(static_cast<int>(point.y - std::round(segment_m_width_v)), 0);
  const auto bottom_right_x =
      std::min(static_cast<int>(point.x + std::round(segment_m_width_h)), static_cast<int>(m_width));
  const auto bottom_right_y =
      std::min(static_cast<int>(point.y + std::round(segment_m_width_v)), static_cast<int>(m_height));

  for (auto i = top_left_x; i < bottom_right_x; ++i)
  {
    for (auto j = top_left_y; j < bottom_right_y; ++j)
    {
      if (island_mask[j * m_width + i] != TerrainType::Water)
      {
        return true;
      }
    }
  }

  return false;
}

void TerrainGenerator::m_flood_fill(const int value, const int x, const int y, std::vector<int>& tiles)
{
  assert(m_valid_coord(x, y));

  const int original_value = tiles[y * m_width + x];
  std::queue<Point<int>> coord_queue;
  std::vector<int> mask(m_width * m_height, TerrainType::Water);

  coord_queue.push(Point<int>(x, y));
  tiles[y * m_width + x] = value;

  while (!coord_queue.empty())
  {
    auto coord = coord_queue.front();
    const int x0 = coord.x;
    const int y0 = coord.y;

    coord_queue.pop();

    // Top coord
    const auto yt = y0 + 1;
    const auto top_coord = yt * m_width + x0;
    if (m_valid_coord(x0, yt) && tiles[top_coord] == original_value && mask[top_coord] == TerrainType::Water)
    {
      mask[top_coord] = TerrainType::Land;
      tiles[top_coord] = value;
      coord_queue.push(Point<int>(x0, yt));
    }

    // Bottom coord
    const auto yb = y0 - 1;
    const auto bottom_coord = yb * m_width + x0;
    if (m_valid_coord(x0, yb) && tiles[bottom_coord] == original_value && mask[bottom_coord] == TerrainType::Water)
    {
      mask[bottom_coord] = TerrainType::Land;
      tiles[bottom_coord] = value;
      coord_queue.push(Point<int>(x0, yb));
    }

    // Left coord
    const auto xl = x0 - 1;
    const auto left_coord = y0 * m_width + xl;
    if (m_valid_coord(xl, y0) && tiles[left_coord] == original_value && mask[left_coord] == TerrainType::Water)
    {
      mask[left_coord] = TerrainType::Land;
      tiles[left_coord] = value;
      coord_queue.push(Point<int>(xl, y0));
    }

    // Right coord
    const auto xr = x0 + 1;
    const auto right_coord = y0 * m_width + xr;
    if (m_valid_coord(xr, y0) && tiles[right_coord] == original_value && mask[right_coord] == TerrainType::Water)
    {
      mask[right_coord] = TerrainType::Land;
      tiles[right_coord] = value;
      coord_queue.push(Point<int>(xr, y0));
    }
  }
}

Point<int> TerrainGenerator::m_get_first_coastline_point(const std::vector<int>& tiles, const IslandData& island)
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

    if (m_is_water(left_point, tiles))
    {
      left_candidate = true;
    }
    if (m_is_water(right_point, tiles))
    {
      right_candidate = true;
    }
    if (m_is_water(bottom_point, tiles))
    {
      bottom_candidate = true;
    }
    if (m_is_water(top_point, tiles))
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
        if (tiles[point.y * m_width + x] != TerrainType::Water)
        {
          is_coast = false;
        }
      }
    }
    if (right_candidate && !is_coast)
    {
      is_coast = true;

      for (auto x = point.x + 1; x < m_width - 1; ++x)
      {
        if (tiles[point.y * m_width + x] != TerrainType::Water)
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
        if (tiles[y * m_width + point.x] != TerrainType::Water)
        {
          is_coast = false;
        }
      }
    }
    if (top_candidate && !is_coast)
    {
      is_coast = true;

      for (auto y = point.y + 1; y < m_height - 1; ++y)
      {
        if (tiles[y * m_width + point.x] != TerrainType::Water)
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
  return Point<int>(m_width, m_height);
}

int TerrainGenerator::m_get_point_value(const Point<int>& point, const std::vector<int>& tiles)
{
  return tiles[point.y * m_width + point.x];
}

bool TerrainGenerator::m_is_coast_point(const Point<int>& point, const std::vector<int>& tiles)
{
  // If point is water
  if (m_get_point_value(point, tiles) == TerrainType::Water)
  {
    return false;
  }

  // Test 8 directions from point
  if (m_is_water(point.left(), tiles))
  {
    return true;
  }
  if (m_is_water(point.right(), tiles))
  {
    return true;
  }
  if (m_is_water(point.bottom(), tiles))
  {
    return true;
  }
  if (m_is_water(point.top(), tiles))
  {
    return true;
  }
  if (m_is_water(point.top_left(), tiles))
  {
    return true;
  }
  if (m_is_water(point.top_right(), tiles))
  {
    return true;
  }
  if (m_is_water(point.bottom_right(), tiles))
  {
    return true;
  }
  if (m_is_water(point.bottom_left(), tiles))
  {
    return true;
  }

  return false;
}

bool TerrainGenerator::m_is_water(const Point<int>& point, const std::vector<int>& tiles)
{
  return m_valid_point(point) && m_get_point_value(point, tiles) == TerrainType::Water;
}

std::vector<Point<int>> TerrainGenerator::m_get_bays(std::vector<Point<int>>& coastline, const IslandData& island)
{
  std::vector<Point<int>> bays;

  const auto minimum_area = m_json.object["coast_min_area"].get<int>();
  const auto points_min_distance = m_json.object["coast_points_min_distance"].get<int>();
  const auto points_max_distance = m_json.object["coast_points_max_distance"].get<int>();

  std::vector<std::pair<Point<int>, Point<int>>> candidates;
  Point<int>* coast_point_a = nullptr;
  Point<int>* coast_point_b = nullptr;

  std::vector<int> area_mask(m_width * m_height, TerrainType::Water);

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

      if (!m_has_land_intersection(point_a, point_b, island.mask, TerrainType::Water))
      {
        found_candidate = true;
        coast_point_a = &point_a;
        coast_point_b = &point_b;
      }
      else if (found_candidate && coast_point_a != nullptr && coast_point_b != nullptr)
      {
        const auto bay_data =
            m_get_bay_data(*coast_point_a, *coast_point_b, island.mask, minimum_area, area_mask, TerrainType::Water);

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

bool TerrainGenerator::m_has_land_intersection(const Point<int>& point_a,
                                               const Point<int>& point_b,
                                               const std::vector<int>& tiles,
                                               const int water_value)
{
  const int middle_x = (point_a.x + point_b.x) / 2;
  const int middle_y = (point_a.y + point_b.y) / 2;

  if (tiles[middle_y * m_width + middle_x] != water_value)
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
    if ((x == static_cast<int>(point_a.x) && y == static_cast<int>(point_a.y)) ||
        (x == static_cast<int>(point_b.x) && y == static_cast<int>(point_b.y)))
    {
      continue;
    }

    if (tiles[y * m_width + x] != water_value)
    {
      --tolerance;
    }

    if (tolerance <= 0)
    {
      return true;
    }
  } while (!TCOD_line_step_mt(&x, &y, &bresenham_data));

  return false;
}

BayData TerrainGenerator::m_get_bay_data(const Point<int>& point_a,
                                         const Point<int>& point_b,
                                         const std::vector<int>& tiles,
                                         const int minimum,
                                         std::vector<int>& mask,
                                         const int water_value)
{
  Point<int> bay_point(point_a.x, point_b.y);

  {
    int x = point_a.x;
    int y = point_a.y;

    TCOD_bresenham_data_t bresenham_data;
    TCOD_line_init_mt(x, y, point_b.x, point_b.y, &bresenham_data);

    do
    {
      if (mask[y * m_width + x] == TerrainType::Land)
      {
        return BayData(0, bay_point);
      }

      mask[y * m_width + x] = TerrainType::Land;
    } while (!TCOD_line_step_mt(&x, &y, &bresenham_data));
  }

  int start_x, start_y;
  const int middle_x = (point_a.x + point_b.x) / 2;
  const int middle_y = (point_a.y + point_b.y) / 2;

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
      const float d = b * middle_x - a * middle_y;

      /* auto perpendicular_x = [a, b, d](int y) */
      /* { */
      /*   (a*y + d) / static_cast<float>(b); */
      /* }; */

      // Find y in function of x
      auto perpendicular_y = [a, b, d](int x) { return (b * x - d) / static_cast<float>(a); };

      for (auto px = middle_x + 1; px < m_width; ++px)
      {
        ++steps_direction_1;
        const auto py = static_cast<int>(std::round(perpendicular_y(px)));

        if (py >= 0 && static_cast<int>(py) < m_height && tiles[py * m_width + px] != water_value)
        {
          if (mask[py * m_width + px] != TerrainType::Water)
          {
            return BayData(0, bay_point);
          }

          // Get previous point
          candidate_a.x = px - 1;
          candidate_a.y = static_cast<int>(std::round(perpendicular_y(px - 1)));
          break;
        }
        else if (py < 2 || static_cast<int>(py) >= m_height)
        {
          steps_direction_1 = m_height + 1;
          break;
        }
      }
      for (auto px = middle_x - 1; px > 0; --px)
      {
        ++steps_direction_2;
        const auto py = static_cast<int>(std::round(perpendicular_y(px)));

        if (py >= 0 && static_cast<int>(py) < m_height && tiles[py * m_width + px] != water_value)
        {
          if (mask[py * m_width + px] != TerrainType::Water)
          {
            return BayData(0, bay_point);
          }

          candidate_b.x = px + 1;
          candidate_b.y = static_cast<int>(std::round(perpendicular_y(px + 1)));
          break;
        }
        else if (py < 2 || static_cast<int>(py) >= m_height || px < 2)
        {
          steps_direction_2 = m_height + 1;
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
  const auto min_land_distance_delta = m_json.object["min_land_distance_delta"].get<int>();

  // Ignore when land is found very close to the center point
  if (delta_candidate_x < min_land_distance_delta && delta_candidate_y < min_land_distance_delta)
  {
    return BayData(0, bay_point);
  }

  // Flood fill ocean area between line and land
  int area = 0;
  std::queue<Point<int>> coord_queue;

  coord_queue.push(Point<int>(start_x, start_y));
  mask[start_y * m_width + start_x] = TerrainType::Land;
  area++;

  while (!coord_queue.empty())
  {
    auto coord = coord_queue.front();
    const int x0 = coord.x;
    const int y0 = coord.y;

    coord_queue.pop();

    // Top coord
    const auto yt = y0 + 1;
    const auto top_coord = yt * m_width + x0;
    if (m_valid_coord(x0, yt) && tiles[top_coord] == water_value && mask[top_coord] == TerrainType::Water)
    {
      mask[top_coord] = TerrainType::Land;
      coord_queue.push(Point<int>(x0, yt));
      area++;
    }

    // Bottom coord
    const auto yb = y0 - 1;
    const auto bottom_coord = yb * m_width + x0;
    if (m_valid_coord(x0, yb) && tiles[bottom_coord] == water_value && mask[bottom_coord] == TerrainType::Water)
    {
      mask[bottom_coord] = TerrainType::Land;
      coord_queue.push(Point<int>(x0, yb));
      area++;
    }

    // Left coord
    const auto xl = x0 - 1;
    const auto left_coord = y0 * m_width + xl;
    if (m_valid_coord(xl, y0) && tiles[left_coord] == water_value && mask[left_coord] == TerrainType::Water)
    {
      mask[left_coord] = TerrainType::Land;
      coord_queue.push(Point<int>(xl, y0));
      area++;
    }

    // Right coord
    const auto xr = x0 + 1;
    const auto right_coord = y0 * m_width + xr;
    if (m_valid_coord(xr, y0) && tiles[right_coord] == water_value && mask[right_coord] == TerrainType::Water)
    {
      mask[right_coord] = TerrainType::Land;
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

void TerrainGenerator::m_build_island_structure(IslandData& island)
{
  assert(island.points.size() > 0 && "Main island size is empty");

  std::array<float, 2> min_point = {static_cast<float>(island.top_left.x - 20),
                                    static_cast<float>(island.top_left.y - 20)};
  std::array<float, 2> max_point = {static_cast<float>(island.bottom_right.x + 20),
                                    static_cast<float>(island.bottom_right.y + 20)};

  const auto poisson_disk_sampling_radius = m_json.object["poisson_disk_sampling_radius"].get<float>();
  const auto poisson_points = thinks::PoissonDiskSampling(poisson_disk_sampling_radius, min_point, max_point);
  std::vector<gal::Vector2<double>> points{};

  // Normalize points to [0.0, 1.0]
  for (const auto& point : poisson_points)
  {
    double normalized_x = point[0] / static_cast<double>(m_width);
    double normalized_y = point[1] / static_cast<double>(m_height);

    points.push_back(gal::Vector2{normalized_x, normalized_y});
  }

  auto algorithm = gal::FortuneAlgorithm<double>(points);
  algorithm.construct();

  island.structure.diagram = algorithm.get_diagram();
  auto& diagram = island.structure.diagram;

  for (const auto& site : diagram.get_sites())
  {
    const auto center = site.point.convert(m_width, m_height);

    // Center is outside island
    if (island.mask[center.y * m_width + center.x] == TerrainType::Water)
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
        const auto origin = half_edge->origin->point.convert(m_width, m_height);
        const auto destination = half_edge->destination->point.convert(m_width, m_height);

        // If any of the edges points lays on water annotate it as coast
        if (island.mask[origin.y * m_width + origin.x] == TerrainType::Water ||
            island.mask[destination.y * m_width + destination.y] == TerrainType::Water)
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
      is_coast = m_center_is_coast(center, island.mask);
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

bool TerrainGenerator::m_center_is_coast(const Point<int>& center, const std::vector<int>& island_mask)
{
  assert(island_mask[center.y * m_width + center.x] == TerrainType::Land && "Center must not be in water");

  int distance_to_water = 0;

  // Check to the left of center
  for (int n = 0; (center.x - n) >= 0; ++n)
  {
    if (island_mask[center.y * m_width + (center.x - n)] == TerrainType::Water)
    {
      distance_to_water = n;
      break;
    }
  }
  // Check to the right of center
  for (int n = 0; (center.x + n) < m_width; ++n)
  {
    if (island_mask[center.y * m_width + (center.x + n)] == TerrainType::Water)
    {
      distance_to_water = std::min(distance_to_water, n);
      break;
    }
  }
  // Check to the bottom of center
  for (int n = 0; (center.y + n) < m_height; ++n)
  {
    if (island_mask[(center.y + n) * m_width + center.x] == TerrainType::Water)
    {
      distance_to_water = std::min(distance_to_water, n);
      break;
    }
  }
  // Check to the top of center
  for (int n = 0; (center.y - n) >= 0; ++n)
  {
    if (island_mask[(center.y - n) * m_width + center.x] == TerrainType::Water)
    {
      distance_to_water = std::min(distance_to_water, n);
      break;
    }
  }
  // Check to the top left of center
  for (int n = 0; (center.y - n) >= 0 && (center.x - n) >= 0; ++n)
  {
    if (island_mask[(center.y - n) * m_width + (center.x - n)] == TerrainType::Water)
    {
      distance_to_water = std::min(distance_to_water, n);
      break;
    }
  }
  // Check to the top right of center
  for (int n = 0; (center.y - n) >= 0 && (center.x + n) < m_width; ++n)
  {
    if (island_mask[(center.y - n) * m_width + (center.x + n)] == TerrainType::Water)
    {
      distance_to_water = std::min(distance_to_water, n);
      break;
    }
  }
  // Check to the bottom right of center
  for (int n = 0; (center.y + n) < m_height && (center.x + n) < m_width; ++n)
  {
    if (island_mask[(center.y + n) * m_width + (center.x + n)] == TerrainType::Water)
    {
      distance_to_water = std::min(distance_to_water, n);
      break;
    }
  }
  // Check to the bottom left of center
  for (int n = 0; (center.y + n) < m_height && (center.x - n) >= 0; ++n)
  {
    if (island_mask[(center.y + n) * m_width + (center.x - n)] == TerrainType::Water)
    {
      distance_to_water = std::min(distance_to_water, n);
      break;
    }
  }

  const auto coast_length = m_json.object["coast_length"].get<int>();
  if (distance_to_water <= coast_length)
  {
    return true;
  }

  return false;
}

void TerrainGenerator::m_generate_main_river(IslandData& island,
                                             std::vector<Point<int>>& bays,
                                             std::vector<int>& tiles,
                                             const int seed)
{
  const auto [source, mouth] = m_get_river_source_and_mouth(island, bays, seed);
  auto river = m_get_river_data(source, mouth);
  m_create_meanders(river, source, mouth, tiles);

  double p1_x = -1.0;
  double p1_y = -1.0;

  // Draw river spline
  for (auto t : river.points_t)
  {
    const auto p2_x = river.spline_x(t);
    const auto p2_y = river.spline_y(t);

    if (p1_x < 0.0 || p1_y < 0.0)
    {
      p1_x = p2_x;
      p1_y = p2_y;
      continue;
    }

    m_draw_line(Point<int>(std::round(p1_x), std::round(p1_y)),
                Point<int>(std::round(p2_x), std::round(p2_y)),
                TerrainType::Water,
                tiles);

    p1_x = p2_x;
    p1_y = p2_y;
  }
}

std::pair<Point<int>, Point<int>> TerrainGenerator::m_get_river_source_and_mouth(IslandData& island,
                                                                                 std::vector<Point<int>>& bays,
                                                                                 const int seed)
{
  assert(bays.size() > 0 && "There are no bays identified");
  assert(island.structure.land_sites.size() > 0 && "There are no land sites");

  random::rng.seed(seed);

  // Leaving it here in case the new random implementation breaks something
  /* std::mt19937 rng(seed); */
  /* std::uniform_int_distribution<unsigned int> land_indexes_dist(0, island.structure.land_sites.size() - 1); */
  /* std::uniform_int_distribution<unsigned int> bay_indexes_dist(0, bays.size() - 1); */
  const auto min_source_mouth_distance_x = m_json.object["min_source_mouth_distance_x"].get<int>();
  const auto min_source_mouth_distance_y = m_json.object["min_source_mouth_distance_y"].get<int>();

  auto source_site = island.structure.land_sites[0];
  auto source_point = source_site->point.convert(m_width, m_height);
  auto& mouth_point = bays[0];

  bool found_river_points = false;
  bool has_reached_first_limit = false;
  bool has_reached_second_limit = false;
  std::size_t tries = 0;

  // Check if line crosses sea water to avoid unrealistic rivers
  auto intersects_water = [this, &island](const Point<int>& source, const Point<int>& mouth)
  {
    int tolerance = 4;
    int x = source.x;
    int y = source.y;

    TCOD_bresenham_data_t bresenham_data;
    TCOD_line_init_mt(x, y, mouth.x, mouth.y, &bresenham_data);

    do
    {
      if (island.mask[y * m_width + x] == TerrainType::Water)
      {
        --tolerance;

        if (tolerance <= 0)
        {
          break;
        }
      }
    } while (!TCOD_line_step_mt(&x, &y, &bresenham_data));

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

    const auto n = random::get_integer(0, island.structure.land_sites.size());

    source_site = island.structure.land_sites[n];
    source_point = Point<int>(source_site->point.x * m_width, source_site->point.y * m_height);

    // Make sure that source point is on the upper part of the island
    if (!has_reached_first_limit && source_point.y > m_height / 2)
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

      if (distance_x >= min_source_mouth_distance_x && distance_y >= min_source_mouth_distance_y &&
          bay.y > source_point.y)
      {
        if (intersects_water(source_point, bay))
        {
          continue;
        }

        mouth_point = bays[i];
        found_river_points = true;
        break;
      }
      else if (has_reached_first_limit && bay.y > source_point.y &&
               (distance_x >= min_source_mouth_distance_x || distance_y >= min_source_mouth_distance_y))
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

RiverData TerrainGenerator::m_get_river_data(const Point<int>& source, const Point<int>& mouth)
{
  RiverData river;
  const auto step = m_json.object["river_bezier_step"].get<double>();

  auto noise = FastNoiseLite{1337};
  noise.SetNoiseType(FastNoiseLite::NoiseType::NoiseType_OpenSimplex2S);
  noise.SetRotationType3D(FastNoiseLite::RotationType3D_ImproveXYPlanes);
  noise.SetFrequency(0.015f);
  noise.SetFractalType(FastNoiseLite::FractalType::FractalType_FBm);
  noise.SetFractalOctaves(5);
  noise.SetFractalLacunarity(2.f);
  noise.SetFractalGain(0.5f);
  noise.SetFractalWeightedStrength(0.f);

  const double distance_x = mouth.x - source.x;
  const double distance_y = mouth.y - source.y;
  std::vector<double> points_x;
  std::vector<double> points_y;

  for (float i = 0.f; i < 1.f; i += step)
  {
    auto point_x = static_cast<double>(source.x + i * distance_x);
    auto point_y = static_cast<double>(source.y + i * distance_y);

    auto noise_value = noise.GetNoise(point_x, point_y);

    if (noise_value < 0.5f)
    {
      point_x += noise_value * 10.f;
      point_y += noise_value * 10.f;
    }
    else
    {
      point_x -= noise_value * 10.f;
      point_y -= noise_value * 10.f;
    }

    points_x.push_back(point_x);
    points_y.push_back(point_y);

    /* m_draw_point(Point<int>(static_cast<int>(point_x), static_cast<int>(point_y)), TerrainType::Red, tiles); */
  }

  // Add mouth points
  points_x.push_back(mouth.x);
  points_y.push_back(mouth.y);

  river.points_t.resize(points_x.size(), 0.0);

  for (std::size_t i = 1; i < points_x.size(); ++i)
  {
    river.points_t[i] = river.points_t[i - 1] + sqrt((points_x[i] - points_x[i - 1]) * (points_x[i] - points_x[i - 1]) +
                                                     (points_y[i] - points_y[i - 1]) * (points_y[i] - points_y[i - 1]));
  }

  river.spline_x.set_points(river.points_t, points_x, tk::spline::cspline_hermite);
  river.spline_y.set_points(river.points_t, points_y, tk::spline::cspline_hermite);
  river.max_length = river.points_t[points_x.size() - 1];

  return river;
}

void TerrainGenerator::m_create_meanders(RiverData& river,
                                         const Point<int>& source,
                                         const Point<int>& mouth,
                                         std::vector<int>& tiles)
{
  (void)mouth;
  std::vector<std::pair<Point<double>, Point<double>>> normals;

  const auto spline_step = m_json.object["river_spline_step"].get<double>();
  const auto min_curvature = m_json.object["river_min_curvature"].get<double>();
  const auto max_curvature = m_json.object["river_max_curvature"].get<double>();
  const auto normal_scale = m_json.object["river_normal_scale"].get<double>();
  const auto tangent_scale = m_json.object["river_tangent_scale"].get<double>();
  const auto bitangent_factor = m_json.object["river_bitangent_factor"].get<double>();
  const auto n_iterations = m_json.object["river_n_iterations"].get<int>();
  const auto curvature_delta = m_json.object["river_curvature_delta"].get<double>();

  const auto draw_normals = m_json.object["temp_draw_normals"].get<bool>();
  const auto draw_tangents = m_json.object["temp_draw_tangents"].get<bool>();
  const auto draw_combined = m_json.object["temp_draw_combined"].get<bool>();

  int n_points = river.max_length / spline_step;
  assert(n_points > 3 && "There should be at least 3 river points for spline");

  std::vector<double> new_points_x(n_points, 0.0);
  std::vector<double> new_points_y(n_points, 0.0);

  for (int iteration = 0; iteration < n_iterations; ++iteration)
  {
    n_points = river.max_length / spline_step;
    assert(n_points > 3 && "There should be at least 3 river points for spline");

    if (static_cast<std::uint32_t>(n_points) != new_points_x.size())
    {
      new_points_x.resize(n_points, 0.0);
      new_points_y.resize(n_points, 0.0);
    }

    int increment_index = 0;

    const auto first_x = river.spline_x(0.0);
    const auto first_y = river.spline_y(0.0);
    const auto first_t = 0.0 - m_distance(Point<double>(source.x, source.y), Point<double>(first_x, first_y));

    for (double t = 0.0 - first_t; t < river.max_length && increment_index < n_points; t += spline_step)
    {
      const auto current_x = river.spline_x(t);
      const auto current_y = river.spline_y(t);

      const auto previous_x = river.spline_x(t - curvature_delta);
      const auto previous_y = river.spline_y(t - curvature_delta);
      const auto next_x = river.spline_x(t + curvature_delta);
      const auto next_y = river.spline_y(t + curvature_delta);

      Point<double> previous_point(previous_x, previous_y);
      Point<double> current_point(current_x, current_y);
      Point<double> next_point(next_x, next_y);

      const auto tangent =
          Point<double>(river.spline_x.deriv(1, t) * tangent_scale, river.spline_y.deriv(1, t) * tangent_scale);
      const auto normal = Point<double>(-tangent.y, tangent.x);

      auto curvature = m_menger_curvature(previous_point,
                                          current_point,
                                          next_point,
                                          m_distance(previous_point, current_point),
                                          m_distance(current_point, next_point),
                                          m_distance(previous_point, next_point));

      if (curvature < 0.0)
      {
        curvature = std::min(curvature, -min_curvature);
        curvature = std::max(curvature, -max_curvature);
      }
      else
      {
        curvature = std::max(curvature, min_curvature);
        curvature = std::min(curvature, max_curvature);
      }

      const double normal_length = curvature * normal_scale;
      const double combined_x =
          (normal.x * normal_length * bitangent_factor + tangent.x * (2.0 - bitangent_factor)) * -1.0;
      const double combined_y =
          (normal.y * normal_length * bitangent_factor + tangent.y * (2.0 - bitangent_factor)) * -1.0;

      /* spdlog::warn("TANGENT: {} {} {}", iteration, tangent.x, tangent.y); */
      /* spdlog::warn("NORMAL: {} {} {}", iteration, normal.x, normal.y); */
      /* spdlog::warn("CURVATURE: {} {} {}", iteration, curvature.x, curvature.y); */
      /* spdlog::warn("COMBINED: {} {} {}", iteration, combined_x, combined_y); */

      new_points_x[increment_index] = current_point.x + combined_x;
      new_points_y[increment_index] = current_point.y + combined_y;

      Point<int> origin(static_cast<int>(std::round(current_point.x)), static_cast<int>(std::round(current_point.y)));
      Point<int> normal_destination(
          static_cast<int>(std::round(current_point.x + normal.x * normal_length * -1.0 * 10.0)),
          static_cast<int>(std::round(current_point.y + normal.y * normal_length * -1.0 * 10.0)));
      Point<int> tangent_destination(static_cast<int>(std::round(current_point.x + tangent.x)),
                                     static_cast<int>(std::round(current_point.y + tangent.y)));
      Point<int> combined_destination(static_cast<int>(std::round(current_point.x + combined_x)),
                                      static_cast<int>(std::round(current_point.y + combined_y)));

      if (iteration == n_iterations - 1)
      {
        if (draw_normals)
        {
          m_draw_line(origin, normal_destination, TerrainType::Yellow, tiles);
        }
        if (draw_tangents)
        {
          m_draw_line(origin, tangent_destination, TerrainType::Yellow, tiles);
        }
        if (draw_combined)
        {
          m_draw_line(origin, combined_destination, TerrainType::Yellow, tiles);
        }
      }

      ++increment_index;
    }

    if (iteration >= n_iterations - 1)
    {
      break;
    }

    if (river.points_t.size() != static_cast<std::size_t>(n_points))
    {
      river.points_t.resize(n_points, 0.0);
    }

    assert(n_points > 0 && "The number of points must be greater than 0");

    river.points_t[0] = 0.0;
    for (int i = 1; i < n_points; ++i)
    {
      const auto increment = sqrt((new_points_x[i] - new_points_x[i - 1]) * (new_points_x[i] - new_points_x[i - 1]) +
                                  (new_points_y[i] - new_points_y[i - 1]) * (new_points_y[i] - new_points_y[i - 1]));
      river.points_t[i] = river.points_t[i - 1] + std::max(increment, 0.0001);
    }

    river.spline_x.set_points(river.points_t, new_points_x, tk::spline::cspline_hermite);
    river.spline_y.set_points(river.points_t, new_points_y, tk::spline::cspline_hermite);
    river.max_length = river.points_t[n_points - 1];
  }
}

double TerrainGenerator::m_distance(const Point<double>& point_a, const Point<double>& point_b)
{
  const double distance_x = point_a.x - point_b.x;
  const double distance_y = point_a.y - point_b.y;
  return sqrt(distance_x * distance_x + distance_y * distance_y);
}

float TerrainGenerator::m_distance(const Point<int>& point_a, const Point<int>& point_b)
{
  const int distance_x = point_a.x - point_b.x;
  const int distance_y = point_a.y - point_b.y;
  return sqrt(distance_x * distance_x + distance_y * distance_y);
}

double TerrainGenerator::m_menger_curvature(const Point<double>& point_a,
                                            const Point<double>& point_b,
                                            const Point<double>& point_c,
                                            const double length_1,
                                            const double length_2,
                                            const double length_3)
{
  assert(length_1 > 0 && "Length must be greater than 0");
  assert(length_2 > 0 && "Length must be greater than 0");
  assert(length_3 > 0 && "Length must be greater than 0");

  return 2.0 * ((point_b.x - point_a.x) * (point_c.y - point_a.y) - (point_b.y - point_a.y) * (point_c.x - point_a.x)) /
         (length_1 * length_2 * length_3);
}

void TerrainGenerator::m_draw_point(const Point<int>& point, const int value, std::vector<int>& tiles)
{
  tiles[point.y * m_width + point.x] = value;
}

void TerrainGenerator::m_draw_big_point(const Point<int>& point, const int value, std::vector<int>& tiles)
{
  tiles[point.y * m_width + point.x] = value;
  tiles[point.left().y * m_width + point.left().x] = value;
  tiles[point.right().y * m_width + point.right().x] = value;
  tiles[point.bottom().y * m_width + point.bottom().x] = value;
  tiles[point.top().y * m_width + point.top().x] = value;
  /* tiles[point.top_left().y*m_width + point.top_left().x] = value; */
  /* tiles[point.top_right().y*m_width + point.top_right().x] = value; */
  /* tiles[point.bottom_left().y*m_width + point.bottom_left().x] = value; */
  /* tiles[point.bottom_right().y*m_width + point.bottom_right().x] = value; */
}

void TerrainGenerator::m_draw_line(const Point<int>& origin,
                                   const Point<int>& destination,
                                   const int value,
                                   std::vector<int>& tiles)
{
  int x = origin.x;
  int y = origin.y;

  TCOD_bresenham_data_t bresenham_data;
  TCOD_line_init_mt(x, y, destination.x, destination.y, &bresenham_data);

  do
  {
    if (x < 0 || y < 0 || x > static_cast<int>(m_width) - 1 || y > static_cast<int>(m_height) - 1)
    {
      continue;
    }

    /* m_draw_big_point(Point<int>(x, y), value, tiles); */
    tiles[y * m_width + x] = value;
  } while (!TCOD_line_step_mt(&x, &y, &bresenham_data));
}
}  // namespace dl
