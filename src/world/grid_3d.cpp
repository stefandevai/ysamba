#include "./grid_3d.hpp"

namespace dl
{
uint32_t Grid3D::at(const int x, const int y, const int z) const
{
  if (x < 0 || x >= size.x || y < 0 || y >= size.y || z < 0 || z >= size.z)
  {
    return 0;
  }

  return values[x + y * size.x + z * size.y * size.x];
}

uint32_t Grid3D::at(const Vector3i& position) const { return at(position.x, position.y, position.z); }

uint32_t Grid3D::at(const Vector3& position) const
{
  return at(static_cast<int>(position.x), static_cast<int>(position.y), static_cast<int>(position.z));
}

void Grid3D::set(const uint32_t id, const int x, const int y, const int z)
{
  if (x < 0 || x >= size.x || y < 0 || y >= size.y || z < 0 || z >= size.z)
  {
    return;
  }

  values[x + y * size.x + z * size.y * size.x] = id;
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
}  // namespace dl
