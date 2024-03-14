#pragma once

#include <cstdint>

#include "core/maths/vector.hpp"

namespace dl
{
struct Position
{
  double x{};
  double y{};
  double z{};
  int spatial_hash_index = 9999;

  Position() = default;
  Position(double x, double y, double z) : x(x), y(y), z(z) {}
  Position(const Vector3& position) : x(position.x), y(position.y), z(position.z) {}
};

template <typename Archive>
void serialize(Archive& archive, Position& position)
{
  archive(position.x, position.y, position.z);
}
}  // namespace dl
