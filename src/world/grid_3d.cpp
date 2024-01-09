#include "./grid_3d.hpp"

#include <spdlog/spdlog.h>

namespace dl
{
const Cell Grid3D::null = Cell{};

uint32_t Grid3D::id_at(const int x, const int y, const int z) const
{
  if (!m_in_bounds(x, y, z))
  {
    // spdlog::debug("OUT OF BOUNDS {} {} {}", x, y, z);
    return 0;
  }

  return values[m_index(x, y, z)].id;
}

uint32_t Grid3D::id_at(const Vector3i& position) const { return id_at(position.x, position.y, position.z); }

uint32_t Grid3D::id_at(const Vector3& position) const
{
  return id_at(static_cast<int>(position.x), static_cast<int>(position.y), static_cast<int>(position.z));
}

const Cell& Grid3D::cell_at(const int x, const int y, const int z) const
{
  if (!m_in_bounds(x, y, z))
  {
    return Grid3D::null;
  }

  return values[m_index(x, y, z)];
}

const Cell& Grid3D::cell_at(const Vector3i& position) const { return cell_at(position.x, position.y, position.z); }

const Cell& Grid3D::cell_at(const Vector3& position) const
{
  return cell_at(static_cast<int>(position.x), static_cast<int>(position.y), static_cast<int>(position.z));
}

void Grid3D::set(const uint32_t id, const int x, const int y, const int z)
{
  if (!m_in_bounds(x, y, z))
  {
    return;
  }

  values[m_index(x, y, z)].id = id;
}

void Grid3D::set(const uint32_t id, const Vector3i& position) { set(id, position.x, position.y, position.z); }

void Grid3D::set(const uint32_t id, const Vector3& position)
{
  set(id, static_cast<int>(position.x), static_cast<int>(position.y), static_cast<int>(position.z));
}

void Grid3D::set_size(const int width, const int height, const int depth)
{
  size.x = width;
  size.y = height;
  size.z = depth;
}

void Grid3D::set_size(const Vector3i& size) { this->size = size; }

bool Grid3D::has_flags(const CellFlag flags, const int x, const int y, const int z) const
{
  if (!m_in_bounds(x, y, z))
  {
    return false;
  }

  const auto& cell = cell_at(x, y, z);
  return (cell.flags & flags) == flags;
}

void Grid3D::set_flags(const CellFlag flags, const int x, const int y, const int z)
{
  if (!m_in_bounds(x, y, z))
  {
    return;
  }

  auto& cell = values[m_index(x, y, z)];
  cell.flags |= flags;
}

void Grid3D::toggle_flags(const CellFlag flags, const int x, const int y, const int z)
{
  if (!m_in_bounds(x, y, z))
  {
    return;
  }

  auto& cell = values[m_index(x, y, z)];
  cell.flags ^= flags;
}

void Grid3D::unset_flags(const CellFlag flags, const int x, const int y, const int z)
{
  if (!m_in_bounds(x, y, z))
  {
    return;
  }

  auto& cell = values[m_index(x, y, z)];
  cell.flags &= ~flags;
}

void Grid3D::reset_flags(const int x, const int y, const int z)
{
  if (!m_in_bounds(x, y, z))
  {
    return;
  }

  auto& cell = values[m_index(x, y, z)];
  cell.flags = DL_CELL_FLAG_NONE;
}

void Grid3D::compute_visibility()
{
  for (int z = 0; z < size.z; ++z)
  {
    for (int y = 0; y < size.y; ++y)
    {
      for (int x = 0; x < size.x; ++x)
      {
        const auto id = id_at(x, y, z);

        if (id == 0)
        {
          continue;
        }

        const bool visible = m_is_any_neighbour_empty(x, y, z);

        if (!visible)
        {
          continue;
        }

        set_flags(DL_CELL_FLAG_VISIBLE, x, y, z);
      }
    }
  }
}

bool Grid3D::m_is_any_neighbour_empty(const int x, const int y, const int z) const
{
  // Check visible tiles in 45deg top down view
  // Top tile
  if (id_at(x, y, z + 1) == 0)
  {
    return true;
  }
  // Diagonal bottom tile
  if (id_at(x, y + 1, z + 1) == 0)
  {
    return true;
  }

  // TODO: After adding view rotation, check all the other directions
  /* if (id_at(x, y + 1, z) == 0) */
  /* { */
  /*   return true; */
  /* } */
  /* if (id_at(x + 1, y, z) == 0) */
  /* { */
  /*   return true; */
  /* } */
  /* if (id_at(x - 1, y, z) == 0) */
  /* { */
  /*   return true; */
  /* } */
  /* if (id_at(x, y - 1, z) == 0) */
  /* { */
  /*   return true; */
  /* } */
  /* if (id_at(x, y, z - 1) == 0) */
  /* { */
  /*   return true; */
  /* } */

  return false;
}

bool Grid3D::is_bottom_empty(const int x, const int y, const int z) const { return id_at(x, y + 1, z) == 0; }

uint32_t Grid3D::m_index(const int x, const int y, const int z) const { return x + y * size.x + z * size.y * size.x; }

bool Grid3D::m_in_bounds(const int x, const int y, const int z) const
{
  if (x < 0 || y < 0 || z < 0 || x >= size.x || y >= size.y || z >= size.z)
  {
    return false;
  }

  return true;
}

}  // namespace dl
