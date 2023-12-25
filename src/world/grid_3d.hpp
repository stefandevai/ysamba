#pragma once

#include <vector>

#include "core/maths/vector.hpp"

namespace dl
{
class Grid3D
{
 public:
  Vector3i size{};
  std::vector<uint32_t> values = std::vector<uint32_t>(size.x * size.y * size.z);
  std::vector<int> height_map = std::vector<int>(size.x * size.y);

  Grid3D() = default;
  Grid3D(const Vector3i& size) : size(size) {}

  uint32_t at(const int x, const int y, const int z) const;
  uint32_t at(const Vector3i& position) const;
  uint32_t at(const Vector3& position) const;
  void set(const uint32_t id, const int x, const int y, const int z);
  void set(const uint32_t id, const Vector3i& position);
  void set(const uint32_t id, const Vector3& position);
  void set_size(const int width, const int height, const int depth);
  void set_size(const Vector3i& size);
};
}  // namespace dl
