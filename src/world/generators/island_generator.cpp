#include "./island_generator.hpp"

#include <FastNoise/FastNoise.h>
#include <heman.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <cmath>
#include <limits>

#include "./terrain_type.hpp"
#include "./utils.hpp"
#include "core/json.hpp"
#include "core/timer.hpp"
#include "world/point.hpp"

namespace
{
double interpolate(double start_1, double end_1, double start_2, const double end_2, double value)
{
  return std::lerp(start_2, end_2, (value - start_1) / (end_1 - start_1));
}
}  // namespace

namespace dl
{
IslandGenerator::IslandGenerator() {}

IslandGenerator::IslandGenerator(const Vector3i& size) : size(size)
{
  m_load_params(m_default_params_filepath);
}

void IslandGenerator::generate(const int seed)
{
  spdlog::info("=============================");
  spdlog::info("= STARTING WORLD GENERATION =");
  spdlog::info("=============================\n");
  spdlog::info("SEED: {}", seed);
  spdlog::info("WIDTH: {}", size.x);
  spdlog::info("HEIGHT: {}\n", size.y);

  Timer timer{};
  timer.start();

  spdlog::info("Generating height map...");

  m_get_height_map(seed);

  spdlog::info("Adjusting islands...");

  timer.stop();
  timer.print("Island generation");

  // TEMP Visualize island mask
  if (island_params.display_mask)
  {
    for (auto i = 0; i < size.x * size.y; ++i)
    {
      height_map[i] = static_cast<uint8_t>(sea_distance_map[i] * 255.0f);

      // if (main_island.mask[i] != TerrainType::Land)
      // {
      //   height_map[i] = 0;
      // }
      // else
      // {
      //   height_map[i] = 100;
      // }
    }
  }
  // TEMP Visualize island mask
}

void IslandGenerator::set_size(const Vector3i& size)
{
  this->size = size;
}

void IslandGenerator::m_load_params(const std::string& filepath)
{
  JSON json{filepath};
  const auto& mask_params = json.object["mask"];

  island_params.frequency = mask_params["frequency"].get<float>();
  island_params.tier_land = mask_params["tier_land"].get<float>();
  island_params.display_mask = mask_params["display_mask"].get<bool>();
  island_params.layer_1_octaves = mask_params["octaves"].get<int>();
  island_params.layer_1_lacunarity = mask_params["lacunarity"].get<float>();
  island_params.layer_1_gain = mask_params["gain"].get<float>();
  island_params.layer_1_weighted_strength = mask_params["weighted_strength"].get<float>();
  island_params.layer_1_terrace_multiplier = mask_params["terrace_multiplier"].get<float>();
}

void IslandGenerator::m_get_height_map(const int seed)
{
  silhouette_map.resize(size.x * size.y);
  island_mask.resize(size.x * size.y);
  mountain_map.resize(size.x * size.y);
  control_map.resize(size.x * size.y);
  height_map.resize(size.x * size.y);
  sea_distance_map.resize(size.x * size.y);

  utils::generate_silhouette_map(silhouette_map.data(), size.x / -2, size.y / -2, size.x, size.y, island_params, seed);
  utils::generate_mountain_map(mountain_map.data(), size.x / -2, size.y / -2, size.x, size.y, island_params, seed + 47);
  utils::generate_control_map(control_map.data(), size.x / -2, size.y / -2, size.x, size.y, island_params, seed + 13);

  const double max_z = size.z - 1.0;
  const float half_size_x = size.x / 2.0f;
  const float half_size_y = size.y / 2.0f;

  const float gradient_diameter = 160.0f;
  const float gradient_diameter_squared = gradient_diameter * gradient_diameter;

  for (int j = 0; j < size.y; ++j)
  {
    for (int i = 0; i < size.x; ++i)
    {
      const auto array_index = j * size.x + i;

      // Apply falloff to the silhouette
      const float distance_x_squared = (half_size_x - i) * (half_size_x - i);
      const float distance_y_squared = (half_size_y - j) * (half_size_y - j);

      float gradient = ((distance_x_squared + distance_y_squared) * 2.0f / gradient_diameter_squared);

      silhouette_map[array_index] -= gradient;
      silhouette_map[array_index] = std::clamp(silhouette_map[array_index], 0.0f, 1.0f);

      double noise_value = silhouette_map[array_index];
      double map_value;

      if (noise_value < 0.3f)
      {
        map_value = interpolate(0.0, 0.3, 0.0, max_z * 0.16, noise_value);
      }
      else if (noise_value < 0.5f)
      {
        map_value = interpolate(0.3, 0.5, max_z * 0.16, max_z * 0.35, noise_value);
      }
      else
      {
        map_value = interpolate(0.5, 1.0, max_z * 0.35, max_z * 0.734, noise_value);
      }

      // Apply mountain map via control map
      if (map_value >= 1.0)
      {
        double mountain_value = mountain_map[array_index];
        double control_value = control_map[array_index];
        auto noise_influence = std::min(1.0, noise_value + 0.5);
        map_value = std::max(map_value, map_value + (mountain_value * control_value * 28.0 * noise_influence));
      }

      // Naive terrace for vizualization
      if (static_cast<int>(map_value) > 0)
      {
        map_value = interpolate(0.0, max_z, 0.0, 255.0, static_cast<int>(map_value));
      }

      map_value = std::clamp(map_value, 0.0, 255.0);
      height_map[array_index] = static_cast<uint8_t>(map_value);

      // Create land mask
      if (height_map[array_index] > 1)
      {
        island_mask[array_index] = TerrainType::Land;
      }
      else
      {
        island_mask[array_index] = TerrainType::None;
      }
    }
  }

  // Floodfill from the borders in order to get a mask of the sea
  bool has_flooded_ocean = false;

  for (int j = 0; j < size.y && !has_flooded_ocean; ++j)
  {
    for (int i = 0; i < size.x && !has_flooded_ocean; ++i)
    {
      if (i != 0 && j != 0)
      {
        continue;
      }

      const auto value = island_mask[j * size.x + i];

      if (value == TerrainType::None)
      {
        if (j == 0 || i == 0)
        {
          m_flood_fill(island_mask, i, j, static_cast<int>(TerrainType::Water));
          has_flooded_ocean = true;
        }
      }
    }
  }

  // Replace inland water with grass
  for (int j = 0; j < size.y; ++j)
  {
    for (int i = 0; i < size.x; ++i)
    {
      if (island_mask[j * size.x + i] == TerrainType::None)
      {
        island_mask[j * size.x + i] = TerrainType::Land;
      }
    }
  }

  // Generate a mask with only the islands we are interested
  auto islands = m_get_islands(island_mask, 2);

  auto island_mask_hi = heman_image_create(size.x, size.y, 1);
  auto island_mask_h = heman_image_data(island_mask_hi);

  for (auto i = 0; i < size.x * size.y; ++i)
  {
    island_mask_h[i] = 1.0f;
  }

  for (const auto& island : islands)
  {
    for (auto i = 0; i < size.x * size.y; ++i)
    {
      if (island.mask[i] == TerrainType::Land)
      {
        island_mask_h[i] = 0.0f;
      }
    }
  }

  // Remove smaller islands from the height map
  for (auto i = 0; i < size.x * size.y; ++i)
  {
    if (island_mask_h[i] == 1.0f)
    {
      height_map[i] = 0;
    }
  }

  // Generate an Unsigned Distance Field relative to the distance to the sea
  auto distance_field = heman_distance_create_df(island_mask_hi);
  memcpy(sea_distance_map.data(), heman_image_data(distance_field), size.x * size.y * sizeof(float));
  heman_image_destroy(distance_field);
  heman_image_destroy(island_mask_hi);
}

void IslandGenerator::m_flood_fill(std::vector<int>& grid, const int x, const int y, const int value_to_fill)
{
  assert(m_valid_coordinate(x, y));

  const int original_value = grid[y * size.x + x];
  std::queue<Point<int>> coordinate_queue;
  std::vector<int> visited(size.x * size.y, value_to_fill);

  coordinate_queue.push(Point<int>(x, y));
  grid[y * size.x + x] = value_to_fill;

  while (!coordinate_queue.empty())
  {
    auto coordinate = coordinate_queue.front();
    const int x0 = coordinate.x;
    const int y0 = coordinate.y;

    coordinate_queue.pop();

    // Top coordinate
    const auto yt = y0 + 1;
    const auto top_coordinate = yt * size.x + x0;
    if (m_valid_coordinate(x0, yt) && grid[top_coordinate] == original_value && visited[top_coordinate] == value_to_fill)
    {
      visited[top_coordinate] = std::numeric_limits<int>::max();
      grid[top_coordinate] = value_to_fill;
      coordinate_queue.push(Point<int>(x0, yt));
    }

    // Bottom coordinate
    const auto yb = y0 - 1;
    const auto bottom_coordinate = yb * size.x + x0;
    if (m_valid_coordinate(x0, yb) && grid[bottom_coordinate] == original_value && visited[bottom_coordinate] == value_to_fill)
    {
      visited[bottom_coordinate] = std::numeric_limits<int>::max();
      grid[bottom_coordinate] = value_to_fill;
      coordinate_queue.push(Point<int>(x0, yb));
    }

    // Left coordinate
    const auto xl = x0 - 1;
    const auto left_coordinate = y0 * size.x + xl;
    if (m_valid_coordinate(xl, y0) && grid[left_coordinate] == original_value && visited[left_coordinate] == value_to_fill)
    {
      visited[left_coordinate] = std::numeric_limits<int>::max();
      grid[left_coordinate] = value_to_fill;
      coordinate_queue.push(Point<int>(xl, y0));
    }

    // Right coordinate
    const auto xr = x0 + 1;
    const auto right_coordinate = y0 * size.x + xr;
    if (m_valid_coordinate(xr, y0) && grid[right_coordinate] == original_value && visited[right_coordinate] == value_to_fill)
    {
      visited[right_coordinate] = std::numeric_limits<int>::max();
      grid[right_coordinate] = value_to_fill;
      coordinate_queue.push(Point<int>(xr, y0));
    }
  }
}

bool IslandGenerator::m_valid_coordinate(const int x, const int y)
{
  return !(x < 0 || y < 0 || x >= size.x || y >= size.y);
}

std::vector<IslandData> IslandGenerator::m_get_islands(std::vector<int>& grid, const uint32_t islands_to_keep)
{
  std::vector<int> mask(size.x * size.y, TerrainType::Water);
  std::vector<IslandData> islands{};

  for (int j = 0; j < size.y; ++j)
  {
    for (int i = 0; i < size.x; ++i)
    {
      const auto tile_value = grid[j * size.x + i];
      const auto mask_value = mask[j * size.x + i];

      // Not water and not masked
      if (tile_value != TerrainType::Water && mask_value == TerrainType::Water)
      {
        const auto island_data = m_get_island(grid, mask, i, j);
        islands.push_back(island_data);
        std::push_heap(islands.begin(), islands.end());
      }
    }
  }

  if (islands.size() > islands_to_keep)
  {
    islands.resize(islands_to_keep);
  }

  return islands;
}

IslandData IslandGenerator::m_get_island(const std::vector<int>& grid,
                                         std::vector<int>& mask,
                                         const int x,
                                         const int y)
{
  assert(m_valid_coordinate(x, y) && "Island coordinates are not valid");

  IslandData island_data;
  island_data.mask.resize(size.x * size.y, TerrainType::Water);

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

    island_data.mask[y0 * size.x + x0] = TerrainType::Land;
    island_data.points.push_back(Point<int>(x0, y0));
    island_data.top_left.x = std::min(x0, island_data.top_left.x);
    island_data.top_left.y = std::min(y0, island_data.top_left.y);
    island_data.bottom_right.x = std::max(x0, island_data.bottom_right.x);
    island_data.bottom_right.y = std::max(y0, island_data.bottom_right.y);

    coord_queue.pop();

    // Top coord
    const auto yt = y0 + 1;
    const auto top_coord = yt * size.x + x0;
    if (m_valid_coordinate(x0, yt) && grid[top_coord] != TerrainType::Water && mask[top_coord] == TerrainType::Water)
    {
      mask[top_coord] = TerrainType::Land;
      coord_queue.push(Point<int>(x0, yt));
    }

    // Bottom coord
    const auto yb = y0 - 1;
    const auto bottom_coord = yb * size.x + x0;
    if (m_valid_coordinate(x0, yb) && grid[bottom_coord] != TerrainType::Water && mask[bottom_coord] == TerrainType::Water)
    {
      mask[bottom_coord] = TerrainType::Land;
      coord_queue.push(Point<int>(x0, yb));
    }

    // Left coord
    const auto xl = x0 - 1;
    const auto left_coord = y0 * size.x + xl;
    if (m_valid_coordinate(xl, y0) && grid[left_coord] != TerrainType::Water && mask[left_coord] == TerrainType::Water)
    {
      mask[left_coord] = TerrainType::Land;
      coord_queue.push(Point<int>(xl, y0));
    }

    // Right coord
    const auto xr = x0 + 1;
    const auto right_coord = y0 * size.x + xr;
    if (m_valid_coordinate(xr, y0) && grid[right_coord] != TerrainType::Water && mask[right_coord] == TerrainType::Water)
    {
      mask[right_coord] = TerrainType::Land;
      coord_queue.push(Point<int>(xr, y0));
    }
  }

  return island_data;
}

}  // namespace dl
