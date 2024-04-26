#pragma once

#include <cstdint>

namespace dl
{
enum CellFlag
{
  DL_CELL_FLAG_NONE = 0,
  DL_CELL_FLAG_TOP_FACE_VISIBLE = 1,
  DL_CELL_FLAG_FRONT_FACE_VISIBLE = 2,
  /* DL_CELL_FLAG_COLLIDABLE = 2, */
  /* DL_CELL_FLAG_TRANSPARENT = 4, */
};

enum class BlockType : uint8_t
{
  None = 0,
  Water = 1,
  Grass = 2,
  Basalt = 3,
  Sand = 4,
};

struct Cell
{
  uint32_t top_face = 0;
  uint32_t front_face = 0;
  uint32_t top_face_decoration = 0;
  uint32_t front_face_decoration = 0;
  uint8_t flags = DL_CELL_FLAG_NONE;
  BlockType block_type = BlockType::None;
};

template <typename Archive>
void serialize(Archive& archive, Cell& cell)
{
  archive(cell.top_face, cell.front_face, cell.top_face_decoration, cell.front_face_decoration, cell.flags, cell.block_type);
}
};  // namespace dl
