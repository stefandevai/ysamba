#include "./grid_3d.hpp"

#include <spdlog/spdlog.h>

namespace dl
{
const Cell Grid3D::null = Cell{};

uint32_t Grid3D::terrain_at(const int x, const int y, const int z) const
{
  if (!m_in_bounds(x, y, z))
  {
    return 0;
  }

  return values[m_index(x, y, z)].terrain;
}

uint32_t Grid3D::terrain_at(const Vector3i& position) const
{
  return terrain_at(position.x, position.y, position.z);
}

uint32_t Grid3D::terrain_at(const Vector3& position) const
{
  return terrain_at(static_cast<int>(position.x), static_cast<int>(position.y), static_cast<int>(position.z));
}

uint32_t Grid3D::decoration_at(const int x, const int y, const int z) const
{
  if (!m_in_bounds(x, y, z))
  {
    return 0;
  }

  return values[m_index(x, y, z)].decoration;
}

uint32_t Grid3D::decoration_at(const Vector3i& position) const
{
  return decoration_at(position.x, position.y, position.z);
}

uint32_t Grid3D::decoration_at(const Vector3& position) const
{
  return decoration_at(static_cast<int>(position.x), static_cast<int>(position.y), static_cast<int>(position.z));
}

const Cell& Grid3D::cell_at(const int x, const int y, const int z) const
{
  if (!m_in_bounds(x, y, z))
  {
    return Grid3D::null;
  }

  return values[m_index(x, y, z)];
}

const Cell& Grid3D::cell_at(const Vector3i& position) const
{
  return cell_at(position.x, position.y, position.z);
}

const Cell& Grid3D::cell_at(const Vector3& position) const
{
  return cell_at(static_cast<int>(position.x), static_cast<int>(position.y), static_cast<int>(position.z));
}

int Grid3D::height_at(const int x, const int y) const
{
  if (!m_in_bounds(x, y))
  {
    return 0;
  }

  return height_map[x + y * size.x];
}

int Grid3D::height_at(const Vector2i& position) const
{
  return height_at(position.x, position.y);
}

int Grid3D::height_at(const Vector2& position) const
{
  return height_at(static_cast<int>(position.x), static_cast<int>(position.y));
}

void Grid3D::set(const uint32_t id, const int x, const int y, const int z)
{
  if (!m_in_bounds(x, y, z))
  {
    return;
  }

  values[m_index(x, y, z)].terrain = id;
}

void Grid3D::set(const uint32_t id, const Vector3i& position)
{
  set(id, position.x, position.y, position.z);
}

void Grid3D::set(const uint32_t id, const Vector3& position)
{
  set(id, static_cast<int>(position.x), static_cast<int>(position.y), static_cast<int>(position.z));
}

void Grid3D::set_decoration(const uint32_t id, const int x, const int y, const int z)
{
  if (!m_in_bounds(x, y, z))
  {
    return;
  }

  values[m_index(x, y, z)].decoration = id;
}

void Grid3D::set_decoration(const uint32_t id, const Vector3i& position)
{
  set_decoration(id, position.x, position.y, position.z);
}

void Grid3D::set_decoration(const uint32_t id, const Vector3& position)
{
  set_decoration(id, static_cast<int>(position.x), static_cast<int>(position.y), static_cast<int>(position.z));
}

void Grid3D::set_height(const int height, const int x, const int y)
{
  if (!m_in_bounds(x, y))
  {
    return;
  }

  height_map[x + y * size.x] = height;
}

void Grid3D::set_height(const int height, const Vector2i& position)
{
  return set_height(height, position.x, position.y);
}

void Grid3D::set_height(const int height, const Vector2& position)
{
  return set_height(height, static_cast<int>(position.x), static_cast<int>(position.y));
}

void Grid3D::set_size(const int width, const int height, const int depth)
{
  size.x = width;
  size.y = height;
  size.z = depth;

  values.resize(size.x * size.y * size.z);
  height_map.resize(size.x * size.y);
}

void Grid3D::set_size(const Vector3i& size)
{
  this->size = size;
  values.resize(size.x * size.y * size.z);
  height_map.resize(size.x * size.y);
}

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
        const auto terrain = terrain_at(x, y, z);
        const auto decoration = decoration_at(x, y, z);

        if (terrain == 0 && decoration == 0)
        {
          continue;
        }

        const bool visible = m_is_any_neighbour_empty(x, y, z);

        if (!visible)
        {
          continue;
        }

        // Update visibility
        set_flags(DL_CELL_FLAG_VISIBLE, x, y, z);

        // Update height map if needed
        if (height_at(x, y) < z)
        {
          set_height(z, x, y);
        }
      }
    }
  }
}

bool Grid3D::m_is_any_neighbour_empty(const int x, const int y, const int z) const
{
  // Check visible tiles in 45deg top down view
  // Top tile
  if (terrain_at(x, y, z + 1) == 0 && decoration_at(x, y, z + 1) == 0)
  {
    return true;
  }
  // Diagonal bottom tile
  // if (terrain_at(x, y + 1, z + 1) == 0)
  // {
  //   return true;
  // }

  // TODO: After adding view rotation, check all the other directions
  /* if (terrain_at(x, y + 1, z) == 0) */
  /* { */
  /*   return true; */
  /* } */
  /* if (terrain_at(x + 1, y, z) == 0) */
  /* { */
  /*   return true; */
  /* } */
  /* if (terrain_at(x - 1, y, z) == 0) */
  /* { */
  /*   return true; */
  /* } */
  /* if (terrain_at(x, y - 1, z) == 0) */
  /* { */
  /*   return true; */
  /* } */
  /* if (terrain_at(x, y, z - 1) == 0) */
  /* { */
  /*   return true; */
  /* } */

  return false;
}

bool Grid3D::is_bottom_empty(const int x, const int y, const int z) const
{
  return terrain_at(x, y + 1, z) == 0;
}

bool Grid3D::has_pattern(const std::vector<uint32_t>& pattern, const Vector2i& size, const Vector3i& position) const
{
  bool found_pattern = true;

  for (int j = 0; j < size.y; ++j)
  {
    for (int i = 0; i < size.x; ++i)
    {
      const auto pattern_value = pattern[j * size.x + i];

      if (pattern_value == 0)
      {
        continue;
      }

      const auto& cell = cell_at(position.x + i, position.y + j, position.z);

      if (cell.decoration == pattern_value)
      {
        continue;
      }

      if (cell.terrain != pattern_value)
      {
        return false;
      }
    }
  }

  return found_pattern;
}

uint32_t Grid3D::m_index(const int x, const int y, const int z) const
{
  return x + y * size.x + z * size.y * size.x;
}

bool Grid3D::m_in_bounds(const int x, const int y, const int z) const
{
  if (x < 0 || y < 0 || z < 0 || x >= size.x || y >= size.y || z >= size.z)
  {
    return false;
  }

  return true;
}

}  // namespace dl
