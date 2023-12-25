#pragma once

namespace dl
{
enum class CellFlag
{
  Visible = 1,
};

struct Cell
{
  uint32_t id = 0;
  uint32_t flags = 0;
};

template <typename Archive>
void serialize(Archive& archive, Cell& cell)
{
  archive(cell.id, cell.flags);
}
};  // namespace dl
