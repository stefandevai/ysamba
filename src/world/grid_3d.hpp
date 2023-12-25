#pragma once

#include <vector>

#include "./cell.hpp"
#include "core/maths/vector.hpp"

namespace dl
{
class Grid3D
{
 public:
  Vector3i size{};
  std::vector<Cell> values = std::vector<Cell>(size.x * size.y * size.z);
  std::vector<int> height_map = std::vector<int>(size.x * size.y);
  static const Cell null;

  Grid3D() = default;
  Grid3D(const Vector3i& size) : size(size) {}

  uint32_t id_at(const int x, const int y, const int z) const;
  uint32_t id_at(const Vector3i& position) const;
  uint32_t id_at(const Vector3& position) const;
  const Cell& cell_at(const int x, const int y, const int z) const;
  const Cell& cell_at(const Vector3i& position) const;
  const Cell& cell_at(const Vector3& position) const;
  void set(const uint32_t id, const int x, const int y, const int z);
  void set(const uint32_t id, const Vector3i& position);
  void set(const uint32_t id, const Vector3& position);
  void set_size(const int width, const int height, const int depth);
  void set_size(const Vector3i& size);
};

template <typename Archive>
void serialize(Archive& archive, Grid3D& grid)
{
  archive(grid.size, grid.values, grid.height_map);
}
}  // namespace dl
