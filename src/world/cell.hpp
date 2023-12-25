#pragma once

namespace dl
{
enum CellFlag
{
  DL_CELL_FLAG_NONE = 0,
  DL_CELL_FLAG_VISIBLE = 1,
  /* DL_CELL_FLAG_COLLIDABLE = 2, */
  /* DL_CELL_FLAG_TRANSPARENT = 4, */
};

struct Cell
{
  uint32_t id = 0;
  uint32_t flags = DL_CELL_FLAG_NONE;
};

template <typename Archive>
void serialize(Archive& archive, Cell& cell)
{
  archive(cell.id, cell.flags);
}
};  // namespace dl
