#include "./chunk_generator.hpp"

#include <FastNoise/FastNoise.h>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <cmath>

#include "constants.hpp"
#include "core/maths/random.hpp"
#include "core/maths/utils.hpp"
#include "world/chunk.hpp"
#include "world/generators/utils.hpp"
#include "world/metadata.hpp"
#include "world/utils.hpp"

namespace dl
{
ChunkGenerator::ChunkGenerator(const WorldMetadata& world_metadata)
    : size(world::chunk_size), m_world_metadata(world_metadata)
{
}

void ChunkGenerator::generate(const int seed, const Vector3i& offset)
{
  m_generate_noise_data(seed, offset);

  chunk = std::make_unique<Chunk>(offset, true);
  chunk->tiles.set_size(size);

  auto terrain = std::vector<int>(m_padded_size.x * m_padded_size.y * size.z);

  for (int j = 0; j < m_padded_size.y; ++j)
  {
    for (int i = 0; i < m_padded_size.x; ++i)
    {
      const auto world_position = offset + Vector3i{i, j, 0};

      const auto biome = m_sample_biome(world_position);
      int k = m_sample_height_map(world_position);

      const auto height_modifier = height_modifier_map[j * m_padded_size.x + i];

      if (height_modifier > 0.4f)
      {
        k += 1;
      }

      k = std::clamp(k, 0, size.z - 1);

      bool inside_chunk = false;

      if (j >= m_padding && j < m_padded_size.x - m_padding && i >= m_padding && i < m_padded_size.y - m_padding)
      {
        inside_chunk = true;
      }

      int terrain_id = 0;
      int resolved_z = k;

      switch (biome)
      {
      case BiomeType::Sea:
      {
        if (k < 1)
        {
          resolved_z = 1;
          terrain_id = 1;
        }
        else
        {
          terrain_id = 2;
        }
        break;
      }
      case BiomeType::Beach:
      {
        if (k < 1)
        {
          resolved_z = 1;
          terrain_id = 1;
        }
        else
        {
          terrain_id = 3;
        }
        break;
      }
      case BiomeType::RockMountains:
      {
        if (k < 1)
        {
          resolved_z = 1;
          terrain_id = 1;
        }
        else
        {
          terrain_id = 4;
        }
        break;
      }
      default:
      {
        if (k < 1)
        {
          resolved_z = 1;
          terrain_id = 1;
        }
        else
        {
          terrain_id = 2;
        }
        break;
      }
      }

      if (inside_chunk)
      {
        chunk->tiles.height_map[(j - 1) * size.x + (i - 1)] = resolved_z;
        chunk->tiles.values[resolved_z * size.x * size.y + (j - 1) * size.x + (i - 1)].top_face = terrain_id;
      }

      terrain[resolved_z * m_padded_size.x * m_padded_size.y + j * m_padded_size.x + i] = terrain_id;

      for (int z = 0; z < resolved_z; ++z)
      {
        terrain[z * m_padded_size.x * m_padded_size.y + j * m_padded_size.x + i] = terrain_id;

        if (inside_chunk)
        {
          chunk->tiles.values[z * size.x * size.y + (j - 1) * size.x + (i - 1)].top_face = terrain_id;
        }
      }
    }
  }

  chunk->tiles.compute_visibility();

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
}

void ChunkGenerator::set_size(const Vector3i& size)
{
  this->size = size;
  m_padded_size = Vector3i{size.x + m_padding * 2, size.y + m_padding * 2, 1};
}

void ChunkGenerator::m_generate_noise_data(const int seed, const Vector3i& offset)
{
  height_modifier_map.resize(m_padded_size.x * m_padded_size.y);
  vegetation_type.resize(size.x * size.y);
  vegetation_density.resize(size.x * size.y);

  // Height modifier
  utils::generate_height_modifier_map(
      height_modifier_map.data(), offset.x - 1, offset.y - 1, m_padded_size.x, m_padded_size.y, seed + 94);

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

void ChunkGenerator::m_select_tile(const std::vector<int>& terrain, const int x, const int y, const int z)
{
  const int transposed_x = x + m_padding;
  const int transposed_y = y + m_padding;

  const auto terrain_id
      = terrain[z * m_padded_size.x * m_padded_size.y + transposed_y * m_padded_size.x + transposed_x];

  if (terrain_id == 0)
  {
    return;
  }

  // TODO: Set non walkable block tile if not visible
  if (!chunk->tiles.has_flags(DL_CELL_FLAG_TOP_FACE_VISIBLE, x, y, z))
  {
    chunk->tiles.values[z * size.x * size.y + y * size.x + x].top_face = 1;
    return;
  }

  const auto& root_rule = TileRules::get_root_rule(terrain_id);

  TileValues old_values{terrain_id, 0};
  TileValues new_values = m_apply_rule(root_rule, terrain, transposed_x, transposed_y, z);

  do
  {
    old_values.top_face = new_values.top_face;
    old_values.top_face_decoration = new_values.top_face_decoration;

    const auto& rule = TileRules::get_terrain_rule(old_values.top_face);
    new_values = m_apply_rule(rule, terrain, transposed_x, transposed_y, z);
  } while (new_values.top_face != 0);

  chunk->tiles.values[z * size.x * size.y + y * size.x + x].top_face = old_values.top_face;

  // Select vegetation
  if (old_values.top_face_decoration == 0)
  {
    old_values.top_face_decoration = m_select_top_face_decoration(terrain_id, x, y, z);
  }

  chunk->tiles.values[z * size.x * size.y + y * size.x + x].top_face_decoration = old_values.top_face_decoration;
}

TileValues ChunkGenerator::m_apply_rule(const Rule& rule_variant, const std::vector<int>& terrain, const int x, const int y, const int z)
{
  const auto index = rule_variant.index();
  TileValues new_values{0, 0};

  switch (index)
  {
  case 0:
  {
    break;
  }

  case 1:
  {
    const auto& rule = std::get<AutoTile4SidesRule>(rule_variant);
    const auto bitmask = m_get_bitmask_4_sided(terrain, x, y, z, rule.neighbor);
    new_values.top_face = rule.output[bitmask].value;
    break;
  }

  case 3:
  {
    const auto& rule = std::get<UniformDistributionRule>(rule_variant);
    const auto prob = random::get_real();
    double cumulative_probability = 0.0;

    for (const auto& transform : rule.output)
    {
      cumulative_probability += transform.probability;

      if (prob < cumulative_probability)
      {
        if (transform.placement == PlacementType::Terrain)
        {
          new_values.top_face = transform.value;
        }
        else
        {
          new_values.top_face_decoration = transform.value;
        }
        break;
      }
    }
    break;
  }

  case 2:
  {
    const auto& rule = std::get<AutoTile8SidesRule>(rule_variant);
    const auto bitmask = m_get_bitmask_8_sided(terrain, x, y, z, rule.neighbor, rule.input);
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

    new_values.top_face = new_terrain_id;
    break;
  }
  default:
    break;
  }

  return new_values;
}

int ChunkGenerator::m_select_top_face_decoration(const int terrain_id, const int x, const int y, const int z)
{
  (void)z;
  int top_face_decoration = 0;

  // TODO: Integrate to the rule system for any terrain
  // Only select top_face_decoration for grass terrain (2)
  if (terrain_id != 2)
  {
    return top_face_decoration;
  }

  // Place bigger plants on the center and smaller ones on the edges
  const auto density = vegetation_density[y * size.x + x];

  if (density < 0.3f)
  {
    return top_face_decoration;
  }

  const auto prob = random::get_real();

  if (prob < 0.4f)
  {
    return top_face_decoration;
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
    top_face_decoration = plant_small;
  }
  else
  {
    top_face_decoration = plant_big;
  }

  return top_face_decoration;
}

uint32_t ChunkGenerator::m_get_bitmask_4_sided(
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

uint32_t ChunkGenerator::m_get_bitmask_8_sided(
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

bool ChunkGenerator::m_has_neighbor(
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

int ChunkGenerator::m_sample_height_map(const Vector3i& world_position)
{
  Vector2 height_map_position = utils::world_to_map(world_position);

  if (!utils::point_aabb(static_cast<Vector2i>(height_map_position), {0, 0}, m_world_metadata.world_size.xy()))
  {
    return 0;
  }

  switch (m_height_map_sampler)
  {
  case Sampler::Nearest:
  {
    const int height_map_index = static_cast<int>(height_map_position.y) * m_world_metadata.world_size.x
                                 + static_cast<int>(height_map_position.x);
    return static_cast<int>(m_world_metadata.height_map[height_map_index] * (size.z - 1));
  }
  case Sampler::Bilinear:
  {
    // Subtract 0.5 to center the sample
    height_map_position -= Vector2{0.5, 0.5};

    const auto x = std::floor(height_map_position.x);
    const auto y = std::floor(height_map_position.y);

    const auto x_ratio = height_map_position.x - x;
    const auto y_ratio = height_map_position.y - y;

    // TODO: Handle edge cases
    const auto height = m_world_metadata.height_map[utils::array_index(x, y, m_world_metadata.world_size.x)];
    const auto height_right = m_world_metadata.height_map[utils::array_index(x + 1, y, m_world_metadata.world_size.x)];
    const auto height_bottom = m_world_metadata.height_map[utils::array_index(x, y + 1, m_world_metadata.world_size.x)];
    const auto height_bottom_right
        = m_world_metadata.height_map[utils::array_index(x + 1, y + 1, m_world_metadata.world_size.x)];

    const auto top = std::lerp(height, height_right, x_ratio);
    const auto bottom = std::lerp(height_bottom, height_bottom_right, x_ratio);

    return static_cast<int>(std::lerp(top, bottom, y_ratio) * (size.z - 1));
  }
  case Sampler::Bicubic:
  {
    // Subtract 0.5 to center the sample
    height_map_position -= Vector2{0.5, 0.5};

    const auto x = std::floor(height_map_position.x);
    const auto y = std::floor(height_map_position.y);

    const auto sample_ratio = Vector2{height_map_position.x - x, height_map_position.y - y};

    std::array<float, 16> samples{};

    for (int j = 0; j < 4; ++j)
    {
      for (int i = 0; i < 4; ++i)
      {
        const auto sample_x = x + i - 1;
        const auto sample_y = y + j - 1;

        if (sample_x < 0 || sample_x >= m_world_metadata.world_size.x || sample_y < 0
            || sample_y >= m_world_metadata.world_size.y)
        {
          samples[j * 4 + i] = 0.0f;
        }
        else
        {
          samples[j * 4 + i]
              = m_world_metadata.height_map[utils::array_index(sample_x, sample_y, m_world_metadata.world_size.x)];
        }
      }
    }

    const auto t = sample_ratio;
    const auto t2 = sample_ratio * sample_ratio;
    const auto t3 = t2 * sample_ratio;

    const auto q1 = 0.5 * (-1.0 * t3 + 2.0 * t2 - t);
    const auto q2 = 0.5 * (3.0 * t3 - 5.0 * t2 + Vector2{2.0, 2.0});
    const auto q3 = 0.5 * (-3.0 * t3 + 4.0 * t2 + t);
    const auto q4 = 0.5 * (t3 - t2);

    std::array<float, 4> rows{};

    for (int j = 0; j < 4; ++j)
    {
      rows[j]
          = q1.x * samples[j * 4] + q2.x * samples[j * 4 + 1] + q3.x * samples[j * 4 + 2] + q4.x * samples[j * 4 + 3];
    }

    const auto interpolated_value = q1.y * rows[0] + q2.y * rows[1] + q3.y * rows[2] + q4.y * rows[3];

    return static_cast<int>(std::clamp(interpolated_value, 0.0, 1.0) * (size.z - 1));
  }
  default:
  {
    break;
  }
  }

  return 0;
}

BiomeType ChunkGenerator::m_sample_biome(const Vector3i& world_position)
{
  Vector2 biome_map_position = utils::world_to_map(world_position);

  if (!utils::point_aabb(static_cast<Vector2i>(biome_map_position), {0, 0}, m_world_metadata.world_size.xy()))
  {
    return BiomeType::Sea;
  }

  return m_world_metadata.biome_map[utils::array_index(
      static_cast<int>(biome_map_position.x), static_cast<int>(biome_map_position.y), m_world_metadata.world_size.x)];
}

}  // namespace dl
