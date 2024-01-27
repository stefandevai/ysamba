#include "./island_generator.hpp"

#include <FastNoise/FastNoise.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <cmath>

#include "./lib/fast_noise_lite.hpp"
#include "./terrain_type.hpp"
#include "core/json.hpp"
#include "world/point.hpp"

namespace dl
{
IslandGenerator::IslandGenerator() { m_load_params(m_default_params_filepath); }

IslandGenerator::IslandGenerator(const int width, const int height, const int depth)
    : width(width), height(height), depth(depth)
{
  IslandGenerator();
}

void IslandGenerator::generate(const int seed)
{
  spdlog::info("=============================");
  spdlog::info("= STARTING WORLD GENERATION =");
  spdlog::info("=============================\n");
  spdlog::info("SEED: {}", seed);
  spdlog::info("WIDTH: {}", width);
  spdlog::info("HEIGHT: {}\n", height);

  auto start = std::chrono::high_resolution_clock::now();

  spdlog::info("Generating height map...");

  raw_height_map.resize(width * height);
  island_mask.resize(width * height);

  m_get_height_map(seed);

  spdlog::info("Adjusting islands...");

  auto islands = m_get_islands(island_mask, 1);
  auto& main_island = islands.back();

  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

  spdlog::info("World generation finished! It took {} milliseconds", duration.count());

  // TEMP Visualize island mask
  for (auto i = 0; i < width * height; ++i)
  {
    if (main_island.mask[i] == TerrainType::Land)
    {
      raw_height_map[i] = 1.0f;
    }
    else
    {
      raw_height_map[i] = 0.0f;
    }
  }
  // TEMP Visualize island mask
}

void IslandGenerator::set_size(const Vector3i& size)
{
  width = size.x;
  height = size.y;
  depth = size.z;
}

void IslandGenerator::m_load_params(const std::string& filepath)
{
  JSON json{filepath};
  const auto& mask_params = json.object["mask"];

  island_params.octaves = mask_params["octaves"].get<int>();
  island_params.frequency = mask_params["frequency"].get<float>();
  island_params.lacunarity = mask_params["lacunarity"].get<float>();
  island_params.gain = mask_params["gain"].get<float>();
  island_params.weighted_strength = mask_params["weighted_strength"].get<float>();
  island_params.tier_land = mask_params["tier_land"].get<float>();
}

void IslandGenerator::m_get_height_map(const int seed)
{
  auto noise = FastNoiseLite{seed};
  noise.SetSeed(seed);
  noise.SetNoiseType(FastNoiseLite::NoiseType::NoiseType_OpenSimplex2S);
  noise.SetFrequency(island_params.frequency);
  noise.SetFractalType(FastNoiseLite::FractalType::FractalType_FBm);
  noise.SetFractalOctaves(island_params.octaves);
  noise.SetFractalLacunarity(island_params.lacunarity);
  noise.SetFractalGain(island_params.gain);
  noise.SetFractalWeightedStrength(island_params.weighted_strength);

  for (int j = 0; j < height; ++j)
  {
    for (int i = 0; i < width; ++i)
    {
      const float gradient = m_get_rectangle_gradient_value(i, j);
      const float noise_value = noise.GetNoise(static_cast<float>(i), static_cast<float>(j)) - gradient;

      int tile_value;

      if (noise_value > island_params.tier_land)
      {
        tile_value = TerrainType::Land;
      }
      else
      {
        tile_value = TerrainType::None;
      }

      island_mask[j * width + i] = tile_value;
    }
  }

  // Using FastNoiseLite temporarily
  // See https://github.com/Auburn/FastNoise2/issues/129
  // const auto& simplex = FastNoise::New<FastNoise::OpenSimplex2S>();
  // const auto& fractal = FastNoise::New<FastNoise::FractalFBm>();
  // fractal->SetSource(simplex);
  // fractal->SetOctaveCount(octaves);
  // fractal->SetLacunarity(lacunarity);
  // fractal->SetGain(gain);
  // fractal->SetWeightedStrength(weighted_strength);
  // fractal->GenUniformGrid2D(raw_height_map.data(), 0, 0, width, height, frequency, seed);
  //
  // for (int j = 0; j < height; ++j)
  // {
  //   for (int i = 0; i < width; ++i)
  //   {
  //     const auto gradient = m_get_rectangle_gradient_value(i, j);
  //     const auto array_index = j * width + i;
  //
  //     raw_height_map[array_index] -= gradient;
  //
  //     if (raw_height_map[array_index] > tier_land)
  //     {
  //       island_mask[array_index] = 2;
  //     }
  //     else
  //     {
  //       island_mask[array_index] = 0;
  //     }
  //   }
  // }

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

      const auto value = island_mask[j * width + i];

      if (value == 0)
      {
        if (j == 0 || i == 0)
        {
          m_flood_fill(1, i, j, island_mask);
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
      if (island_mask[j * width + i] == 0)
      {
        island_mask[j * width + i] = 2;
      }
    }
  }
}

double IslandGenerator::m_get_rectangle_gradient_value(const int x, const int y)
{
  auto distance_to_edge = std::min(abs(x - width), x);
  distance_to_edge = std::min(distance_to_edge, abs(y - height));
  distance_to_edge = std::min(distance_to_edge, y) * 2;
  return 1.0 - static_cast<double>(distance_to_edge) / (width / 2.0);
}

void IslandGenerator::m_flood_fill(const int value, const int x, const int y, std::vector<int>& tiles)
{
  assert(m_valid_coord(x, y));

  const int original_value = tiles[y * width + x];
  std::queue<Point<int>> coord_queue;
  std::vector<int> mask(width * height, value);

  coord_queue.push(Point<int>(x, y));
  tiles[y * width + x] = value;

  while (!coord_queue.empty())
  {
    auto coord = coord_queue.front();
    const int x0 = coord.x;
    const int y0 = coord.y;

    coord_queue.pop();

    // Top coord
    const auto yt = y0 + 1;
    const auto top_coord = yt * width + x0;
    if (m_valid_coord(x0, yt) && tiles[top_coord] == original_value && mask[top_coord] == value)
    {
      mask[top_coord] = INFINITY;
      tiles[top_coord] = value;
      coord_queue.push(Point<int>(x0, yt));
    }

    // Bottom coord
    const auto yb = y0 - 1;
    const auto bottom_coord = yb * width + x0;
    if (m_valid_coord(x0, yb) && tiles[bottom_coord] == original_value && mask[bottom_coord] == value)
    {
      mask[bottom_coord] = INFINITY;
      tiles[bottom_coord] = value;
      coord_queue.push(Point<int>(x0, yb));
    }

    // Left coord
    const auto xl = x0 - 1;
    const auto left_coord = y0 * width + xl;
    if (m_valid_coord(xl, y0) && tiles[left_coord] == original_value && mask[left_coord] == value)
    {
      mask[left_coord] = INFINITY;
      tiles[left_coord] = value;
      coord_queue.push(Point<int>(xl, y0));
    }

    // Right coord
    const auto xr = x0 + 1;
    const auto right_coord = y0 * width + xr;
    if (m_valid_coord(xr, y0) && tiles[right_coord] == original_value && mask[right_coord] == value)
    {
      mask[right_coord] = INFINITY;
      tiles[right_coord] = value;
      coord_queue.push(Point<int>(xr, y0));
    }
  }
}

bool IslandGenerator::m_valid_coord(const int x, const int y) { return !(x < 0 || y < 0 || x >= width || y >= height); }

std::vector<IslandData> IslandGenerator::m_get_islands(std::vector<int>& tiles, const uint32_t islands_to_keep)
{
  auto all_islands = m_get_island_queue(tiles);

  while (all_islands.size() > islands_to_keep)
  {
    const auto& current_island = all_islands.top();

    for (const auto& coord : current_island.points)
    {
      // Set island values to water to erase them
      tiles[coord.y * width + coord.x] = TerrainType::Water;
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

IslandQueue IslandGenerator::m_get_island_queue(const std::vector<int>& tiles)
{
  std::vector<int> mask(width * height, TerrainType::Water);
  IslandQueue islands{};

  for (int j = 0; j < height; ++j)
  {
    for (int i = 0; i < width; ++i)
    {
      const auto tile_value = tiles[j * width + i];
      const auto mask_value = mask[j * width + i];

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

IslandData IslandGenerator::m_get_island(const std::vector<int>& tiles,
                                         std::vector<int>& mask,
                                         const int x,
                                         const int y)
{
  assert(m_valid_coord(x, y) && "Island coordinates are not valid");

  IslandData island_data;
  island_data.mask.resize(width * height, TerrainType::Water);

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

    island_data.mask[y0 * width + x0] = TerrainType::Land;
    island_data.points.push_back(Point<int>(x0, y0));
    island_data.top_left.x = std::min(x0, island_data.top_left.x);
    island_data.top_left.y = std::min(y0, island_data.top_left.y);
    island_data.bottom_right.x = std::max(x0, island_data.bottom_right.x);
    island_data.bottom_right.y = std::max(y0, island_data.bottom_right.y);

    coord_queue.pop();

    // Top coord
    const auto yt = y0 + 1;
    const auto top_coord = yt * width + x0;
    if (m_valid_coord(x0, yt) && tiles[top_coord] != TerrainType::Water && mask[top_coord] == TerrainType::Water)
    {
      mask[top_coord] = TerrainType::Land;
      coord_queue.push(Point<int>(x0, yt));
    }

    // Bottom coord
    const auto yb = y0 - 1;
    const auto bottom_coord = yb * width + x0;
    if (m_valid_coord(x0, yb) && tiles[bottom_coord] != TerrainType::Water && mask[bottom_coord] == TerrainType::Water)
    {
      mask[bottom_coord] = TerrainType::Land;
      coord_queue.push(Point<int>(x0, yb));
    }

    // Left coord
    const auto xl = x0 - 1;
    const auto left_coord = y0 * width + xl;
    if (m_valid_coord(xl, y0) && tiles[left_coord] != TerrainType::Water && mask[left_coord] == TerrainType::Water)
    {
      mask[left_coord] = TerrainType::Land;
      coord_queue.push(Point<int>(xl, y0));
    }

    // Right coord
    const auto xr = x0 + 1;
    const auto right_coord = y0 * width + xr;
    if (m_valid_coord(xr, y0) && tiles[right_coord] != TerrainType::Water && mask[right_coord] == TerrainType::Water)
    {
      mask[right_coord] = TerrainType::Land;
      coord_queue.push(Point<int>(xr, y0));
    }
  }

  return island_data;
}

}  // namespace dl
