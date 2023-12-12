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
  uint32_t spatial_hash_index = 99999;
};

template <typename Archive>
void serialize(Archive& archive, Position& position)
{
  archive(position.x, position.y, position.z);
}
}  // namespace dl
