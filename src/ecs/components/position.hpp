#pragma once

#include <cstdint>
#include <limits>

#include "core/maths/vector.hpp"

namespace dl
{
struct Position
{
  double x{};
  double y{};
  double z{};
  uint32_t spatial_hash_index = std::numeric_limits<uint32_t>::infinity();

  Position() = default;
  Position(const double x, const double y, const double z) : x(x), y(y), z(z) {}
  Position(const Vector3& position) : x(position.x), y(position.y), z(position.z) {}
};

template <typename Archive>
void serialize(Archive& archive, Position& position)
{
  archive(position.x, position.y, position.z);
}
}  // namespace dl
