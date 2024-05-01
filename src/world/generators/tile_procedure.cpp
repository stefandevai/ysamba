#include "./tile_procedure.hpp"

#include "world/generators/tile_procedure_manager.hpp"

namespace dl
{
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
