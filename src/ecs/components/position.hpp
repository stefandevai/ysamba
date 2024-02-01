#pragma once

#include <cstdint>

#include "core/maths/vector.hpp"

namespace dl
{
struct Position
{
  double x;
  double y;
  double z;
  int spatial_hash_index = INFINITY;
};

template <typename Archive>
void serialize(Archive& archive, Position& position)
{
  archive(position.x, position.y, position.z);
}
}  // namespace dl
