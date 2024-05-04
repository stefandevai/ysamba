#include "./tile_procedure.hpp"

#include "world/generators/tile_procedure_manager.hpp"

namespace dl
{
void AutotileFourSidesHorizontal::apply(TileProcedureData& data)
{
  if (source != nullptr)
  {
    source->apply(data);
  }

  const auto bitmask = m_get_bitmask(data);

  data.cell->top_face = bitmask_values[bitmask];
}

uint32_t AutotileFourSidesHorizontal::m_get_bitmask(TileProcedureData& data)
{
  uint32_t bitmask = 0;
  const auto& terrain = *data.terrain;
  const auto& padded_size = *data.padded_size;
  const auto& position = *data.cell_position;

  // Top
  if (position.y > 0
      && terrain[position.z * padded_size.x * padded_size.y + (position.y - 1) * padded_size.x + position.x]
             == neighbor)
  {
    bitmask |= DL_EDGE_TOP;
  }
  // Right
  if (position.x < padded_size.x - 1
      && terrain[position.z * padded_size.x * padded_size.y + position.y * padded_size.x + position.x + 1]
             == neighbor)
  {
    bitmask |= DL_EDGE_RIGHT;
  }
  // Bottom
  if (position.y < padded_size.y - 1
      && terrain[position.z * padded_size.x * padded_size.y + (position.y + 1) * padded_size.x + position.x]
             == neighbor)
  {
    bitmask |= DL_EDGE_BOTTOM;
  }
  // Left
  if (position.x > 0
      && terrain[position.z * padded_size.x * padded_size.y + position.y * padded_size.x + position.x - 1]
             == neighbor)
  {
    bitmask |= DL_EDGE_LEFT;
  }

  return bitmask;
}

void AutotileEightSidesHorizontal::apply(TileProcedureData& data)
{
  if (source != nullptr)
  {
    source->apply(data);
  }

  const auto bitmask = m_get_bitmask(data);

  int new_terrain_id = 0;

  switch (bitmask)
  {
  case DL_EDGE_RIGHT | DL_EDGE_BOTTOM_RIGHT | DL_EDGE_BOTTOM:
    new_terrain_id = bitmask_values[0];
    break;
  case DL_EDGE_LEFT | DL_EDGE_BOTTOM_LEFT | DL_EDGE_BOTTOM | DL_EDGE_BOTTOM_RIGHT | DL_EDGE_RIGHT:
    new_terrain_id = bitmask_values[1];
    break;
  case DL_EDGE_LEFT | DL_EDGE_BOTTOM_LEFT | DL_EDGE_BOTTOM:
    new_terrain_id = bitmask_values[2];
    break;
  case DL_EDGE_TOP | DL_EDGE_TOP_RIGHT | DL_EDGE_RIGHT | DL_EDGE_BOTTOM_RIGHT | DL_EDGE_BOTTOM:
    new_terrain_id = bitmask_values[3];
    break;
  case DL_EDGE_TOP_LEFT | DL_EDGE_TOP | DL_EDGE_TOP_RIGHT | DL_EDGE_RIGHT | DL_EDGE_BOTTOM_RIGHT | DL_EDGE_BOTTOM
      | DL_EDGE_BOTTOM_LEFT | DL_EDGE_LEFT:
    new_terrain_id = bitmask_values[4];
    break;
  case DL_EDGE_TOP | DL_EDGE_TOP_LEFT | DL_EDGE_LEFT | DL_EDGE_BOTTOM_LEFT | DL_EDGE_BOTTOM:
    new_terrain_id = bitmask_values[5];
    break;
  case DL_EDGE_TOP | DL_EDGE_TOP_RIGHT | DL_EDGE_RIGHT:
    new_terrain_id = bitmask_values[6];
    break;
  case DL_EDGE_LEFT | DL_EDGE_TOP_LEFT | DL_EDGE_TOP | DL_EDGE_TOP_RIGHT | DL_EDGE_RIGHT:
    new_terrain_id = bitmask_values[7];
    break;
  case DL_EDGE_LEFT | DL_EDGE_TOP_LEFT | DL_EDGE_TOP:
    new_terrain_id = bitmask_values[8];
    break;
  case DL_EDGE_RIGHT:
    new_terrain_id = bitmask_values[9];
    break;
  case DL_EDGE_LEFT | DL_EDGE_RIGHT:
    new_terrain_id = bitmask_values[10];
    break;
  case DL_EDGE_LEFT:
    new_terrain_id = bitmask_values[11];
    break;
  case DL_EDGE_BOTTOM:
    new_terrain_id = bitmask_values[12];
    break;
  case DL_EDGE_TOP | DL_EDGE_BOTTOM:
    new_terrain_id = bitmask_values[13];
    break;
  case DL_EDGE_TOP:
    new_terrain_id = bitmask_values[14];
    break;
  case DL_EDGE_NONE:
    new_terrain_id = bitmask_values[15];
    break;
  case DL_EDGE_BOTTOM | DL_EDGE_RIGHT:
    new_terrain_id = bitmask_values[16];
    break;
  case DL_EDGE_LEFT | DL_EDGE_BOTTOM_LEFT | DL_EDGE_BOTTOM | DL_EDGE_RIGHT:
    new_terrain_id = bitmask_values[17];
    break;
  case DL_EDGE_LEFT | DL_EDGE_BOTTOM | DL_EDGE_BOTTOM_RIGHT | DL_EDGE_RIGHT:
    new_terrain_id = bitmask_values[18];
    break;
  case DL_EDGE_LEFT | DL_EDGE_BOTTOM:
    new_terrain_id = bitmask_values[19];
    break;
  case DL_EDGE_BOTTOM | DL_EDGE_RIGHT | DL_EDGE_TOP_RIGHT | DL_EDGE_TOP:
    new_terrain_id = bitmask_values[20];
    break;
  case DL_EDGE_LEFT | DL_EDGE_BOTTOM_LEFT | DL_EDGE_BOTTOM | DL_EDGE_RIGHT | DL_EDGE_TOP_RIGHT | DL_EDGE_TOP
      | DL_EDGE_TOP_LEFT:
    new_terrain_id = bitmask_values[21];
    break;
  case DL_EDGE_LEFT | DL_EDGE_BOTTOM | DL_EDGE_BOTTOM_RIGHT | DL_EDGE_RIGHT | DL_EDGE_TOP_RIGHT | DL_EDGE_TOP
      | DL_EDGE_TOP_LEFT:
    new_terrain_id = bitmask_values[22];
    break;
  case DL_EDGE_TOP | DL_EDGE_TOP_LEFT | DL_EDGE_LEFT | DL_EDGE_BOTTOM:
    new_terrain_id = bitmask_values[23];
    break;
  case DL_EDGE_TOP | DL_EDGE_RIGHT | DL_EDGE_BOTTOM_RIGHT | DL_EDGE_BOTTOM:
    new_terrain_id = bitmask_values[24];
    break;
  case DL_EDGE_TOP | DL_EDGE_RIGHT | DL_EDGE_BOTTOM_RIGHT | DL_EDGE_BOTTOM | DL_EDGE_BOTTOM_LEFT | DL_EDGE_LEFT
      | DL_EDGE_TOP_LEFT:
    new_terrain_id = bitmask_values[25];
    break;
  case DL_EDGE_TOP | DL_EDGE_TOP_RIGHT | DL_EDGE_RIGHT | DL_EDGE_BOTTOM_RIGHT | DL_EDGE_BOTTOM | DL_EDGE_BOTTOM_LEFT
      | DL_EDGE_LEFT:
    new_terrain_id = bitmask_values[26];
    break;
  case DL_EDGE_TOP | DL_EDGE_BOTTOM | DL_EDGE_BOTTOM_LEFT | DL_EDGE_LEFT:
    new_terrain_id = bitmask_values[27];
    break;
  case DL_EDGE_TOP | DL_EDGE_RIGHT:
    new_terrain_id = bitmask_values[28];
    break;
  case DL_EDGE_TOP | DL_EDGE_RIGHT | DL_EDGE_LEFT | DL_EDGE_TOP_LEFT | DL_EDGE_TOP:
    new_terrain_id = bitmask_values[29];
    break;
  case DL_EDGE_TOP | DL_EDGE_TOP_RIGHT | DL_EDGE_RIGHT | DL_EDGE_LEFT:
    new_terrain_id = bitmask_values[30];
    break;
  case DL_EDGE_TOP | DL_EDGE_LEFT:
    new_terrain_id = bitmask_values[31];
    break;
  case DL_EDGE_TOP | DL_EDGE_RIGHT | DL_EDGE_BOTTOM:
    new_terrain_id = bitmask_values[32];
    break;
  case DL_EDGE_TOP | DL_EDGE_RIGHT | DL_EDGE_BOTTOM | DL_EDGE_BOTTOM_LEFT | DL_EDGE_LEFT | DL_EDGE_TOP_LEFT:
    new_terrain_id = bitmask_values[33];
    break;
  case DL_EDGE_TOP | DL_EDGE_TOP_RIGHT | DL_EDGE_RIGHT | DL_EDGE_BOTTOM_RIGHT | DL_EDGE_BOTTOM | DL_EDGE_LEFT:
    new_terrain_id = bitmask_values[34];
    break;
  case DL_EDGE_TOP | DL_EDGE_LEFT | DL_EDGE_BOTTOM:
    new_terrain_id = bitmask_values[35];
    break;
  case DL_EDGE_LEFT | DL_EDGE_BOTTOM | DL_EDGE_RIGHT:
    new_terrain_id = bitmask_values[36];
    break;
  case DL_EDGE_LEFT | DL_EDGE_TOP_LEFT | DL_EDGE_TOP | DL_EDGE_TOP_RIGHT | DL_EDGE_RIGHT | DL_EDGE_BOTTOM:
    new_terrain_id = bitmask_values[37];
    break;
  case DL_EDGE_TOP | DL_EDGE_RIGHT | DL_EDGE_BOTTOM_RIGHT | DL_EDGE_BOTTOM | DL_EDGE_BOTTOM_LEFT | DL_EDGE_LEFT:
    new_terrain_id = bitmask_values[38];
    break;
  case DL_EDGE_LEFT | DL_EDGE_TOP | DL_EDGE_RIGHT:
    new_terrain_id = bitmask_values[39];
    break;
  case DL_EDGE_TOP | DL_EDGE_RIGHT | DL_EDGE_BOTTOM | DL_EDGE_LEFT:
    new_terrain_id = bitmask_values[40];
    break;
  case DL_EDGE_LEFT | DL_EDGE_TOP_LEFT | DL_EDGE_TOP | DL_EDGE_RIGHT | DL_EDGE_BOTTOM_RIGHT | DL_EDGE_BOTTOM:
    new_terrain_id = bitmask_values[41];
    break;
  case DL_EDGE_TOP | DL_EDGE_TOP_RIGHT | DL_EDGE_RIGHT | DL_EDGE_BOTTOM | DL_EDGE_BOTTOM_LEFT | DL_EDGE_LEFT:
    new_terrain_id = bitmask_values[42];
    break;
  case DL_EDGE_TOP | DL_EDGE_RIGHT | DL_EDGE_BOTTOM_RIGHT | DL_EDGE_BOTTOM | DL_EDGE_LEFT:
    new_terrain_id = bitmask_values[43];
    break;
  case DL_EDGE_TOP | DL_EDGE_TOP_RIGHT | DL_EDGE_RIGHT | DL_EDGE_BOTTOM | DL_EDGE_LEFT:
    new_terrain_id = bitmask_values[44];
    break;
  case DL_EDGE_TOP | DL_EDGE_RIGHT | DL_EDGE_BOTTOM | DL_EDGE_BOTTOM_LEFT | DL_EDGE_LEFT:
    new_terrain_id = bitmask_values[45];
    break;
  case DL_EDGE_TOP | DL_EDGE_RIGHT | DL_EDGE_BOTTOM | DL_EDGE_LEFT | DL_EDGE_TOP_LEFT:
    new_terrain_id = bitmask_values[46];
    break;
  }

  if (new_terrain_id == 0)
  {
    spdlog::warn("Could not find a matching tile for bitmask {}", bitmask);
  }

  data.cell->top_face = new_terrain_id;
}

uint32_t AutotileEightSidesHorizontal::m_get_bitmask(TileProcedureData& data)
{
  uint32_t bitmask = 0;
  const auto& terrain = *data.terrain;
  const auto& padded_size = *data.padded_size;
  const auto& position = *data.cell_position;

  // Top
  if (position.y > 0
      && terrain[position.z * padded_size.x * padded_size.y + (position.y - 1) * padded_size.x + position.x]
             == neighbor)
  {
    bitmask |= DL_EDGE_TOP;
  }
  // Right
  if (position.x < padded_size.x - 1
      && terrain[position.z * padded_size.x * padded_size.y + position.y * padded_size.x + position.x + 1]
             == neighbor)
  {
    bitmask |= DL_EDGE_RIGHT;
  }
  // Bottom
  if (position.y < padded_size.y - 1
      && terrain[position.z * padded_size.x * padded_size.y + (position.y + 1) * padded_size.x + position.x]
             == neighbor)
  {
    bitmask |= DL_EDGE_BOTTOM;
  }
  // Left
  if (position.x > 0
      && terrain[position.z * padded_size.x * padded_size.y + position.y * padded_size.x + position.x - 1]
             == neighbor)
  {
    bitmask |= DL_EDGE_LEFT;
  }
  // Top Left
  if (position.x > 0 && position.y > 0
      && terrain[position.z * padded_size.x * padded_size.y + (position.y - 1) * padded_size.x + position.x - 1]
             == neighbor)
  {
    bitmask |= DL_EDGE_TOP_LEFT;
  }
  // Top Right
  if (position.x < padded_size.x - 1 && position.y > 0
      && terrain[position.z * padded_size.x * padded_size.y + (position.y - 1) * padded_size.x + position.x + 1]
             == neighbor)
  {
    bitmask |= DL_EDGE_TOP_RIGHT;
  }
  // Bottom Right
  if (position.x < padded_size.x - 1 && position.y < padded_size.y - 1
      && terrain[position.z * padded_size.x * padded_size.y + (position.y + 1) * padded_size.x + position.x + 1]
             == neighbor)
  {
    bitmask |= DL_EDGE_BOTTOM_RIGHT;
  }
  // Bottom Left
  if (position.x > 0 && position.y < padded_size.y - 1
      && terrain[position.z * padded_size.x * padded_size.y + (position.y + 1) * padded_size.x + position.x - 1]
             == neighbor)
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

void GenerateTerrainChunk::set_source(TileProcedureNode* source)
{
  this->source = source;
}

void GenerateTerrainChunk::set_padding(const int padding)
{
  this->padding = padding;
}

void GenerateTerrainChunk::apply(TileProcedureData& data)
{
  if (source != nullptr)
  {
    source->apply(data);
  }

  const auto& size = data.chunk->tiles.size;
  auto& tiles = data.chunk->tiles;
  auto padded_size = Vector3i{size.x + padding * 2, size.y + padding * 2, size.z};
  auto& terrain = *data.terrain;

  for (int z = 0; z < size.z; ++z)
  {
    for (int y = 0; y < size.y; ++y)
    {
      for (int x = 0; x < size.x; ++x)
      {
        const bool top_face_visible = tiles.has_flags(DL_CELL_FLAG_TOP_FACE_VISIBLE, x, y, z);
        const bool front_face_visible = tiles.has_flags(DL_CELL_FLAG_FRONT_FACE_VISIBLE, x, y, z);
        const auto block_type = terrain[z * size.x * size.y + y * size.x + x];

        if (!top_face_visible && !front_face_visible)
        {
          continue;
        }

        auto procedure = TileProcedureManager::get_by_block(block_type);
        auto& cell = tiles.values[z * size.x * size.y + y * size.x + x];
        Vector3i cell_position = {x + padding, y + padding, z};
        TileProcedureData cell_data = {
            .chunk = data.chunk,
            .cell = &cell,
            .cell_position = &cell_position,
            .padded_size = &padded_size,
            .terrain = data.terrain,
        };
        procedure->apply(cell_data);
      }
    }
  }
}
}  // namespace dl
