#include "./game_chunk_generator.hpp"

#include <FastNoise/FastNoise.h>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <chrono>
#include <cmath>

#include "./tile_rules.hpp"
#include "./utils.hpp"
#include "constants.hpp"
#include "core/random.hpp"
#include "core/timer.hpp"
#include "world/chunk.hpp"

namespace
{
double interpolate(double start_1, double end_1, double start_2, const double end_2, double value)
{
  return std::lerp(start_2, end_2, (value - start_1) / (end_1 - start_1));
}
}  // namespace

namespace dl
{
GameChunkGenerator::GameChunkGenerator() : GameChunkGenerator(world::chunk_size) {}

GameChunkGenerator::GameChunkGenerator(const Vector3i& size) : size(size)
{
  // map_to_tiles = static_cast<float>(world::chunk_size.x);
  const float frequency = 0.016f / map_to_tiles;

  island_params.layer_1_octaves = 4;
  island_params.frequency = frequency;
  island_params.layer_1_lacunarity = 3.2f;
  island_params.layer_1_gain = 0.32f;
  island_params.layer_1_weighted_strength = 0.25f;
  island_params.layer_1_terrace_multiplier = 0.14f;

  island_params.layer_2_seed_offset = 7;
  island_params.layer_2_octave_count = 4;
  island_params.layer_2_lacunarity = 1.42f;
  island_params.layer_2_gain = 1.1f;
  island_params.layer_2_weighted_strength = 0.42f;
  island_params.layer_2_fade = 0.62f;
  island_params.layer_2_smooth_rhs = -0.68f;
  island_params.layer_2_smoothness = 1.76f;
}

void GameChunkGenerator::generate(const int seed, const Vector3i& offset)
{
  // spdlog::info("====================");
  // spdlog::info("= GENERATING CHUNK =");
  // spdlog::info("====================\n");
  // spdlog::info("SEED: {}", seed);
  // spdlog::info("WIDTH: {}", width);
  // spdlog::info("HEIGHT: {}\n", size.y);

  // Timer timer{};

  chunk = std::make_unique<Chunk>(offset, true);
  chunk->tiles.set_size(size);

  auto terrain = std::vector<int>(m_padded_size.x * m_padded_size.y * size.z);

  // timer.start();

  // spdlog::info("Generating height maps...");

  m_get_height_map(seed, offset);

  // spdlog::info("Setting terrain...");

  // for (int j = 0; j < m_padded_size.y; ++j)
  // {
  //   for (int i = 0; i < m_padded_size.x; ++i)
  //   {
  //     const auto map_value = std::clamp(raw_height_map[j * m_padded_size.x + i], 0.0f, 1.0f);
  //     const int k = static_cast<int>(map_value * (size.z - 1));
  //     bool inside_chunk = false;
  //
  //     if (j >= m_generation_padding && j < m_padded_size.x - m_generation_padding && i >= m_generation_padding
  //         && i < m_padded_size.y - m_generation_padding)
  //     {
  //       inside_chunk = true;
  //     }
  //
  //     int terrain_id = 0;
  //     int resolved_z = k;
  //
  //     if (k < 1)
  //     {
  //       terrain_id = 1;
  //       ++resolved_z;
  //     }
  //     else
  //     {
  //       terrain_id = 2;
  //     }
  //
  //     if (inside_chunk)
  //     {
  //       chunk->tiles.height_map[(j - 1) * size.x + (i - 1)] = resolved_z;
  //       chunk->tiles.values[resolved_z * size.x * size.y + (j - 1) * size.x + (i - 1)].terrain = terrain_id;
  //     }
  //
  //     terrain[resolved_z * m_padded_size.x * m_padded_size.y + j * m_padded_size.x + i] = terrain_id;
  //
  //     for (int z = 0; z < resolved_z; ++z)
  //     {
  //       terrain[z * m_padded_size.x * m_padded_size.y + j * m_padded_size.x + i] = terrain_id;
  //
  //       if (inside_chunk)
  //       {
  //         chunk->tiles.values[z * size.x * size.y + (j - 1) * size.x + (i - 1)].terrain = terrain_id;
  //       }
  //     }
  //   }
  // }

  const float gradient_diameter = 160.0f * map_to_tiles;
  const float gradient_diameter_squared = gradient_diameter * gradient_diameter;

  for (int j = 0; j < m_padded_size.y; ++j)
  {
    for (int i = 0; i < m_padded_size.x; ++i)
    {
      const auto array_index = j * m_padded_size.x + i;

      const float distance_x_squared = (i + offset.x) * (i + offset.x);
      const float distance_y_squared = (j + offset.y) * (j + offset.y);
      float gradient = ((distance_x_squared + distance_y_squared) * 2.0f / gradient_diameter_squared);
      silhouette_map[array_index] -= gradient;
      silhouette_map[array_index] = std::clamp(silhouette_map[array_index], 0.0f, 1.0f);
      // silhouette_map[array_index] = std::clamp(gradient, 0.0f, 1.0f);

      const double noise_value = silhouette_map[array_index];
      const double max_z = size.z - 1.0;

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

      // const double mountain_value = mountain_map[array_index];

      // if (mountain_value > noise_value)
      // {
      //   map_value = map_value * (mountain_value * 2.0);
      // }

      map_value = std::clamp(map_value, 0.0, max_z);

      // const auto map_value = std::clamp(raw_height_map[j * m_padded_size.x + i], 0.0f, 1.0f);
      const int k = static_cast<int>(map_value);
      // const int k = static_cast<int>(silhouette_map[j * m_padded_size.x + i] * (size.z - 1));
      bool inside_chunk = false;

      if (j >= m_generation_padding && j < m_padded_size.x - m_generation_padding && i >= m_generation_padding
          && i < m_padded_size.y - m_generation_padding)
      {
        inside_chunk = true;
      }

      int terrain_id = 0;
      int resolved_z = k;

      if (k < 6)
      {
        terrain_id = 1;
        resolved_z = 6;
      }
      else
      {
        terrain_id = 2;
      }

      if (inside_chunk)
      {
        chunk->tiles.height_map[(j - 1) * size.x + (i - 1)] = resolved_z;
        chunk->tiles.values[resolved_z * size.x * size.y + (j - 1) * size.x + (i - 1)].terrain = terrain_id;
      }

      terrain[resolved_z * m_padded_size.x * m_padded_size.y + j * m_padded_size.x + i] = terrain_id;

      for (int z = 0; z < resolved_z; ++z)
      {
        terrain[z * m_padded_size.x * m_padded_size.y + j * m_padded_size.x + i] = terrain_id;

        if (inside_chunk)
        {
          chunk->tiles.values[z * size.x * size.y + (j - 1) * size.x + (i - 1)].terrain = terrain_id;
        }
      }
    }
  }

  // spdlog::info("Computing visibility...");

  chunk->tiles.compute_visibility();

  // spdlog::info("Selecting tiles...");

  for (int k = 0; k < size.z; ++k)
  {
    for (int j = 0; j < size.y; ++j)
    {
      for (int i = 0; i < size.x; ++i)
      {
        m_select_tile(terrain, i, j, k);
      }
    }
  }

  // timer.stop();
  // timer.print("Chunk generation");
}

void GameChunkGenerator::set_size(const Vector3i& size)
{
  this->size = size;
  m_padded_size = Vector3i{size.x + m_generation_padding * 2, size.y + m_generation_padding * 2, 1};
}

void GameChunkGenerator::m_get_height_map(const int seed, const Vector3i& offset)
{
  // height_map.resize(size.x * size.y);
  silhouette_map.resize(m_padded_size.x * m_padded_size.y);
  mountain_map.resize(m_padded_size.x * m_padded_size.y);
  // raw_height_map.resize(m_padded_size.x * m_padded_size.y);
  vegetation_type.resize(size.x * size.y);
  vegetation_density.resize(size.x * size.y);

  // const auto generator = utils::get_island_noise_generator(island_params);
  // FastNoise::SmartNode<> generator
  //
  // return generator;
  //     =
  //     FastNoise::NewFromEncodedNodeTree("FwAAAIC/AACAPwAAAL8AAIA/GgABDQAFAAAArkchQCkAAHsULj8AmpkZPwEFAAEAAAAAAAAAAAAAAAAAAAAAAAAA");

  // generator->GenUniformGrid2D(raw_height_map.data(),
  //                             offset.x - m_generation_padding,
  //                             offset.y + m_generation_padding,
  //                             // offset.x - m_generation_padding - 256 / 2 * map_to_tiles,
  //                             // offset.y + m_generation_padding - 256 / 2 * map_to_tiles,
  //                             size.x + m_generation_padding * 2,
  //                             size.y + m_generation_padding * 2,
  //                             island_params.frequency,
  //                             seed);

  utils::generate_silhouette_map(silhouette_map.data(),
                                 offset.x - m_generation_padding,
                                 offset.y + m_generation_padding,
                                 size.x + m_generation_padding * 2,
                                 size.y + m_generation_padding * 2,
                                 island_params,
                                 seed);

  utils::generate_mountain_map(mountain_map.data(),
                               offset.x - m_generation_padding,
                               offset.y + m_generation_padding,
                               size.x + m_generation_padding * 2,
                               size.y + m_generation_padding * 2,
                               island_params,
                               seed + 47);

  // Vegetation type lookup
  FastNoise::SmartNode<> vegetation_type_noise
      = FastNoise::NewFromEncodedNodeTree("DAADAAAA7FG4Pw0AAwAAAAAAAEApAAAAAAA/AAAAAAAAAAAgQA==");
  vegetation_type_noise->GenUniformGrid2D(vegetation_type.data(), offset.x, offset.y, size.x, size.y, 0.05f, seed + 30);

  // Vegetation density lookup
  FastNoise::SmartNode<> vegetation_density_noise
      = FastNoise::NewFromEncodedNodeTree("DQACAAAAexROQCkAAFK4Hj8AmpkZPw==");
  vegetation_density_noise->GenUniformGrid2D(
      vegetation_density.data(), offset.x, offset.y, size.x, size.y, 0.05f, seed + 50);
}

void GameChunkGenerator::m_select_tile(const std::vector<int>& terrain, const int x, const int y, const int z)
{
  const int transposed_x = x + m_generation_padding;
  const int transposed_y = y + m_generation_padding;

  const auto terrain_id
      = terrain[z * m_padded_size.x * m_padded_size.y + transposed_y * m_padded_size.x + transposed_x];

  if (terrain_id == 0)
  {
    return;
  }

  // TODO: Set non walkable block tile if not visible
  if (!chunk->tiles.has_flags(DL_CELL_FLAG_VISIBLE, x, y, z))
  {
    chunk->tiles.values[z * size.x * size.y + y * size.x + x].terrain = 1;
    return;
  }

  TileValues old_values{terrain_id, 0};
  TileValues new_values{terrain_id, 0};

  do
  {
    old_values.terrain = new_values.terrain;
    old_values.decoration = new_values.decoration;

    const auto& rule_object = TileRules::get(old_values.terrain);
    const auto index = rule_object.index();

    switch (index)
    {
    case 0:
      break;

    case 1:
    {
      const auto& rule = std::get<AutoTile4SidesRule>(rule_object);
      const auto bitmask = m_get_bitmask_4_sided(terrain, transposed_x, transposed_y, z, rule.neighbor);
      new_values.terrain = rule.output[bitmask].value;
      break;
    }

    case 3:
    {
      const auto& rule = std::get<UniformDistributionRule>(rule_object);
      const auto prob = random::get_real();
      double cumulative_probability = 0.0;

      for (const auto& transform : rule.output)
      {
        cumulative_probability += transform.probability;

        if (prob < cumulative_probability)
        {
          if (transform.placement == PlacementType::Terrain)
          {
            new_values.terrain = transform.value;
          }
          else
          {
            new_values.decoration = transform.value;
          }
          break;
        }
      }
      break;
    }

    case 2:
    {
      const auto& rule = std::get<AutoTile8SidesRule>(rule_object);
      const auto bitmask = m_get_bitmask_8_sided(terrain, transposed_x, transposed_y, z, rule.neighbor, rule.input);
      int new_terrain_id = 0;

      switch (bitmask)
      {
      case DL_EDGE_RIGHT | DL_EDGE_BOTTOM_RIGHT | DL_EDGE_BOTTOM:
        new_terrain_id = rule.output[0].value;
        break;
      case DL_EDGE_LEFT | DL_EDGE_BOTTOM_LEFT | DL_EDGE_BOTTOM | DL_EDGE_BOTTOM_RIGHT | DL_EDGE_RIGHT:
        new_terrain_id = rule.output[1].value;
        break;
      case DL_EDGE_LEFT | DL_EDGE_BOTTOM_LEFT | DL_EDGE_BOTTOM:
        new_terrain_id = rule.output[2].value;
        break;
      case DL_EDGE_TOP | DL_EDGE_TOP_RIGHT | DL_EDGE_RIGHT | DL_EDGE_BOTTOM_RIGHT | DL_EDGE_BOTTOM:
        new_terrain_id = rule.output[3].value;
        break;
      case DL_EDGE_TOP_LEFT | DL_EDGE_TOP | DL_EDGE_TOP_RIGHT | DL_EDGE_RIGHT | DL_EDGE_BOTTOM_RIGHT | DL_EDGE_BOTTOM
          | DL_EDGE_BOTTOM_LEFT | DL_EDGE_LEFT:
        new_terrain_id = rule.output[4].value;
        break;
      case DL_EDGE_TOP | DL_EDGE_TOP_LEFT | DL_EDGE_LEFT | DL_EDGE_BOTTOM_LEFT | DL_EDGE_BOTTOM:
        new_terrain_id = rule.output[5].value;
        break;
      case DL_EDGE_TOP | DL_EDGE_TOP_RIGHT | DL_EDGE_RIGHT:
        new_terrain_id = rule.output[6].value;
        break;
      case DL_EDGE_LEFT | DL_EDGE_TOP_LEFT | DL_EDGE_TOP | DL_EDGE_TOP_RIGHT | DL_EDGE_RIGHT:
        new_terrain_id = rule.output[7].value;
        break;
      case DL_EDGE_LEFT | DL_EDGE_TOP_LEFT | DL_EDGE_TOP:
        new_terrain_id = rule.output[8].value;
        break;
      case DL_EDGE_RIGHT:
        new_terrain_id = rule.output[9].value;
        break;
      case DL_EDGE_LEFT | DL_EDGE_RIGHT:
        new_terrain_id = rule.output[10].value;
        break;
      case DL_EDGE_LEFT:
        new_terrain_id = rule.output[11].value;
        break;
      case DL_EDGE_BOTTOM:
        new_terrain_id = rule.output[12].value;
        break;
      case DL_EDGE_TOP | DL_EDGE_BOTTOM:
        new_terrain_id = rule.output[13].value;
        break;
      case DL_EDGE_TOP:
        new_terrain_id = rule.output[14].value;
        break;
      case DL_EDGE_NONE:
        new_terrain_id = rule.output[15].value;
        break;
      case DL_EDGE_BOTTOM | DL_EDGE_RIGHT:
        new_terrain_id = rule.output[16].value;
        break;
      case DL_EDGE_LEFT | DL_EDGE_BOTTOM_LEFT | DL_EDGE_BOTTOM | DL_EDGE_RIGHT:
        new_terrain_id = rule.output[17].value;
        break;
      case DL_EDGE_LEFT | DL_EDGE_BOTTOM | DL_EDGE_BOTTOM_RIGHT | DL_EDGE_RIGHT:
        new_terrain_id = rule.output[18].value;
        break;
      case DL_EDGE_LEFT | DL_EDGE_BOTTOM:
        new_terrain_id = rule.output[19].value;
        break;
      case DL_EDGE_BOTTOM | DL_EDGE_RIGHT | DL_EDGE_TOP_RIGHT | DL_EDGE_TOP:
        new_terrain_id = rule.output[20].value;
        break;
      case DL_EDGE_LEFT | DL_EDGE_BOTTOM_LEFT | DL_EDGE_BOTTOM | DL_EDGE_RIGHT | DL_EDGE_TOP_RIGHT | DL_EDGE_TOP
          | DL_EDGE_TOP_LEFT:
        new_terrain_id = rule.output[21].value;
        break;
      case DL_EDGE_LEFT | DL_EDGE_BOTTOM | DL_EDGE_BOTTOM_RIGHT | DL_EDGE_RIGHT | DL_EDGE_TOP_RIGHT | DL_EDGE_TOP
          | DL_EDGE_TOP_LEFT:
        new_terrain_id = rule.output[22].value;
        break;
      case DL_EDGE_TOP | DL_EDGE_TOP_LEFT | DL_EDGE_LEFT | DL_EDGE_BOTTOM:
        new_terrain_id = rule.output[23].value;
        break;
      case DL_EDGE_TOP | DL_EDGE_RIGHT | DL_EDGE_BOTTOM_RIGHT | DL_EDGE_BOTTOM:
        new_terrain_id = rule.output[24].value;
        break;
      case DL_EDGE_TOP | DL_EDGE_RIGHT | DL_EDGE_BOTTOM_RIGHT | DL_EDGE_BOTTOM | DL_EDGE_BOTTOM_LEFT | DL_EDGE_LEFT
          | DL_EDGE_TOP_LEFT:
        new_terrain_id = rule.output[25].value;
        break;
      case DL_EDGE_TOP | DL_EDGE_TOP_RIGHT | DL_EDGE_RIGHT | DL_EDGE_BOTTOM_RIGHT | DL_EDGE_BOTTOM | DL_EDGE_BOTTOM_LEFT
          | DL_EDGE_LEFT:
        new_terrain_id = rule.output[26].value;
        break;
      case DL_EDGE_TOP | DL_EDGE_BOTTOM | DL_EDGE_BOTTOM_LEFT | DL_EDGE_LEFT:
        new_terrain_id = rule.output[27].value;
        break;
      case DL_EDGE_TOP | DL_EDGE_RIGHT:
        new_terrain_id = rule.output[28].value;
        break;
      case DL_EDGE_TOP | DL_EDGE_RIGHT | DL_EDGE_LEFT | DL_EDGE_TOP_LEFT | DL_EDGE_TOP:
        new_terrain_id = rule.output[29].value;
        break;
      case DL_EDGE_TOP | DL_EDGE_TOP_RIGHT | DL_EDGE_RIGHT | DL_EDGE_LEFT:
        new_terrain_id = rule.output[30].value;
        break;
      case DL_EDGE_TOP | DL_EDGE_LEFT:
        new_terrain_id = rule.output[31].value;
        break;
      case DL_EDGE_TOP | DL_EDGE_RIGHT | DL_EDGE_BOTTOM:
        new_terrain_id = rule.output[32].value;
        break;
      case DL_EDGE_TOP | DL_EDGE_RIGHT | DL_EDGE_BOTTOM | DL_EDGE_BOTTOM_LEFT | DL_EDGE_LEFT | DL_EDGE_TOP_LEFT:
        new_terrain_id = rule.output[33].value;
        break;
      case DL_EDGE_TOP | DL_EDGE_TOP_RIGHT | DL_EDGE_RIGHT | DL_EDGE_BOTTOM_RIGHT | DL_EDGE_BOTTOM | DL_EDGE_LEFT:
        new_terrain_id = rule.output[34].value;
        break;
      case DL_EDGE_TOP | DL_EDGE_LEFT | DL_EDGE_BOTTOM:
        new_terrain_id = rule.output[35].value;
        break;
      case DL_EDGE_LEFT | DL_EDGE_BOTTOM | DL_EDGE_RIGHT:
        new_terrain_id = rule.output[36].value;
        break;
      case DL_EDGE_LEFT | DL_EDGE_TOP_LEFT | DL_EDGE_TOP | DL_EDGE_TOP_RIGHT | DL_EDGE_RIGHT | DL_EDGE_BOTTOM:
        new_terrain_id = rule.output[37].value;
        break;
      case DL_EDGE_TOP | DL_EDGE_RIGHT | DL_EDGE_BOTTOM_RIGHT | DL_EDGE_BOTTOM | DL_EDGE_BOTTOM_LEFT | DL_EDGE_LEFT:
        new_terrain_id = rule.output[38].value;
        break;
      case DL_EDGE_LEFT | DL_EDGE_TOP | DL_EDGE_RIGHT:
        new_terrain_id = rule.output[39].value;
        break;
      case DL_EDGE_TOP | DL_EDGE_RIGHT | DL_EDGE_BOTTOM | DL_EDGE_LEFT:
        new_terrain_id = rule.output[40].value;
        break;
      case DL_EDGE_LEFT | DL_EDGE_TOP_LEFT | DL_EDGE_TOP | DL_EDGE_RIGHT | DL_EDGE_BOTTOM_RIGHT | DL_EDGE_BOTTOM:
        new_terrain_id = rule.output[41].value;
        break;
      case DL_EDGE_TOP | DL_EDGE_TOP_RIGHT | DL_EDGE_RIGHT | DL_EDGE_BOTTOM | DL_EDGE_BOTTOM_LEFT | DL_EDGE_LEFT:
        new_terrain_id = rule.output[42].value;
        break;
      case DL_EDGE_TOP | DL_EDGE_RIGHT | DL_EDGE_BOTTOM_RIGHT | DL_EDGE_BOTTOM | DL_EDGE_LEFT:
        new_terrain_id = rule.output[43].value;
        break;
      case DL_EDGE_TOP | DL_EDGE_TOP_RIGHT | DL_EDGE_RIGHT | DL_EDGE_BOTTOM | DL_EDGE_LEFT:
        new_terrain_id = rule.output[44].value;
        break;
      case DL_EDGE_TOP | DL_EDGE_RIGHT | DL_EDGE_BOTTOM | DL_EDGE_BOTTOM_LEFT | DL_EDGE_LEFT:
        new_terrain_id = rule.output[45].value;
        break;
      case DL_EDGE_TOP | DL_EDGE_RIGHT | DL_EDGE_BOTTOM | DL_EDGE_LEFT | DL_EDGE_TOP_LEFT:
        new_terrain_id = rule.output[46].value;
        break;
      }

      if (new_terrain_id == 0)
      {
        spdlog::warn("Could not find a matching tile for bitmask {}", bitmask);
      }

      new_values.terrain = new_terrain_id;
      break;
    }
    default:
      break;
    }

  } while (old_values.terrain != new_values.terrain);

  chunk->tiles.values[z * size.x * size.y + y * size.x + x].terrain = new_values.terrain;

  // Select vegetation
  if (new_values.decoration == 0)
  {
    new_values.decoration = m_select_decoration(terrain_id, x, y, z);
  }

  chunk->tiles.values[z * size.x * size.y + y * size.x + x].decoration = new_values.decoration;
}

int GameChunkGenerator::m_select_decoration(const int terrain_id, const int x, const int y, const int z)
{
  (void)z;
  int decoration = 0;

  // TODO: Integrate to the rule system for any terrain
  // Only select decoration for grass terrain (2)
  if (terrain_id != 2)
  {
    return decoration;
  }

  // Place bigger plants on the center and smaller ones on the edges
  const auto density = vegetation_density[y * size.x + x];

  if (density < 0.3f)
  {
    return decoration;
  }

  const auto prob = random::get_real();

  if (prob < 0.4f)
  {
    return decoration;
  }

  int plant_small = 0;
  int plant_big = 0;

  const auto type_threshold = vegetation_type[y * size.x + x];

  if (type_threshold < 0.0f)
  {
    plant_small = 54;
    plant_big = 53;
  }
  else
  {
    plant_small = 49;
    plant_big = 48;
  }

  if (density < 0.4f)
  {
    decoration = plant_small;
  }
  else
  {
    decoration = plant_big;
  }

  return decoration;
}

uint32_t GameChunkGenerator::m_get_bitmask_4_sided(
    const std::vector<int>& terrain, const int x, const int y, const int z, const int neighbor)
{
  uint32_t bitmask = 0;

  // Top
  if (y > 0 && terrain[z * m_padded_size.x * m_padded_size.y + (y - 1) * m_padded_size.x + x] == neighbor)
  {
    bitmask |= DL_EDGE_TOP;
  }
  // Right
  if (x < m_padded_size.x - 1
      && terrain[z * m_padded_size.x * m_padded_size.y + y * m_padded_size.x + x + 1] == neighbor)
  {
    bitmask |= DL_EDGE_RIGHT;
  }
  // Bottom
  if (y < m_padded_size.y - 1
      && terrain[z * m_padded_size.x * m_padded_size.y + (y + 1) * m_padded_size.x + x] == neighbor)
  {
    bitmask |= DL_EDGE_BOTTOM;
  }
  // Left
  if (x > 0 && terrain[z * m_padded_size.x * m_padded_size.y + y * m_padded_size.x + x - 1] == neighbor)
  {
    bitmask |= DL_EDGE_LEFT;
  }

  return bitmask;
}

uint32_t GameChunkGenerator::m_get_bitmask_8_sided(
    const std::vector<int>& terrain, const int x, const int y, const int z, const int neighbor, const int source)
{
  if (!m_has_neighbor(terrain, x, y, z, neighbor))
  {
    return DL_EDGE_TOP | DL_EDGE_RIGHT | DL_EDGE_BOTTOM | DL_EDGE_LEFT | DL_EDGE_TOP_LEFT | DL_EDGE_TOP_RIGHT
           | DL_EDGE_BOTTOM_RIGHT | DL_EDGE_BOTTOM_LEFT;
  }

  uint32_t bitmask = 0;

  // Top
  if (y > 0 && terrain[z * m_padded_size.x * m_padded_size.y + (y - 1) * m_padded_size.x + x] == source)
  {
    bitmask |= DL_EDGE_TOP;
  }
  // Right
  if (x < m_padded_size.x - 1 && terrain[z * m_padded_size.x * m_padded_size.y + y * m_padded_size.x + x + 1] == source)
  {
    bitmask |= DL_EDGE_RIGHT;
  }
  // Bottom
  if (y < m_padded_size.y - 1
      && terrain[z * m_padded_size.x * m_padded_size.y + (y + 1) * m_padded_size.x + x] == source)
  {
    bitmask |= DL_EDGE_BOTTOM;
  }
  // Left
  if (x > 0 && terrain[z * m_padded_size.x * m_padded_size.y + y * m_padded_size.x + x - 1] == source)
  {
    bitmask |= DL_EDGE_LEFT;
  }
  // Top Left
  if (x > 0 && y > 0 && terrain[z * m_padded_size.x * m_padded_size.y + (y - 1) * m_padded_size.x + x - 1] == source)
  {
    bitmask |= DL_EDGE_TOP_LEFT;
  }
  // Top Right
  if (x < m_padded_size.x - 1 && y > 0
      && terrain[z * m_padded_size.x * m_padded_size.y + (y - 1) * m_padded_size.x + x + 1] == source)
  {
    bitmask |= DL_EDGE_TOP_RIGHT;
  }
  // Bottom Right
  if (x < m_padded_size.x - 1 && y < m_padded_size.y - 1
      && terrain[z * m_padded_size.x * m_padded_size.y + (y + 1) * m_padded_size.x + x + 1] == source)
  {
    bitmask |= DL_EDGE_BOTTOM_RIGHT;
  }
  // Bottom Left
  if (x > 0 && y < m_padded_size.y - 1
      && terrain[z * m_padded_size.x * m_padded_size.y + (y + 1) * m_padded_size.x + x - 1] == source)
  {
    bitmask |= DL_EDGE_BOTTOM_LEFT;
  }

  if (!(bitmask & DL_EDGE_LEFT) || !(bitmask & DL_EDGE_TOP))
  {
    bitmask &= ~DL_EDGE_TOP_LEFT;
  }
  if (!(bitmask & DL_EDGE_LEFT) || !(bitmask & DL_EDGE_BOTTOM))
  {
    bitmask &= ~DL_EDGE_BOTTOM_LEFT;
  }
  if (!(bitmask & DL_EDGE_RIGHT) || !(bitmask & DL_EDGE_TOP))
  {
    bitmask &= ~DL_EDGE_TOP_RIGHT;
  }
  if (!(bitmask & DL_EDGE_RIGHT) || !(bitmask & DL_EDGE_BOTTOM))
  {
    bitmask &= ~DL_EDGE_BOTTOM_RIGHT;
  }

  return bitmask;
}

bool GameChunkGenerator::m_has_neighbor(
    const std::vector<int>& terrain, const int x, const int y, const int z, const int neighbor)
{
  // Top
  if (y > 0 && terrain[z * m_padded_size.x * m_padded_size.y + (y - 1) * m_padded_size.x + x] == neighbor)
  {
    return true;
  }
  // Right
  if (x < m_padded_size.x - 1
      && terrain[z * m_padded_size.x * m_padded_size.y + y * m_padded_size.x + x + 1] == neighbor)
  {
    return true;
  }
  // Bottom
  if (y < m_padded_size.y - 1
      && terrain[z * m_padded_size.x * m_padded_size.y + (y + 1) * m_padded_size.x + x] == neighbor)
  {
    return true;
  }
  // Left
  if (x > 0 && terrain[z * m_padded_size.x * m_padded_size.y + y * m_padded_size.x + x - 1] == neighbor)
  {
    return true;
  }
  // Top Left
  if (x > 0 && y > 0 && terrain[z * m_padded_size.x * m_padded_size.y + (y - 1) * m_padded_size.x + x - 1] == neighbor)
  {
    return true;
  }
  // Top Right
  if (x < m_padded_size.x - 1 && y > 0
      && terrain[z * m_padded_size.x * m_padded_size.y + (y - 1) * m_padded_size.x + x + 1] == neighbor)
  {
    return true;
  }
  // Bottom Right
  if (x < m_padded_size.x - 1 && y < m_padded_size.y - 1
      && terrain[z * m_padded_size.x * m_padded_size.y + (y + 1) * m_padded_size.x + x + 1] == neighbor)
  {
    return true;
  }
  // Bottom Left
  if (x > 0 && y < m_padded_size.y - 1
      && terrain[z * m_padded_size.x * m_padded_size.y + (y + 1) * m_padded_size.x + x - 1] == neighbor)
  {
    return true;
  }

  return false;
}

}  // namespace dl
