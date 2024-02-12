#pragma once

#include <cstdint>

#include "core/maths/vector.hpp"

namespace dl
{
struct Movement
{
  Vector3 direction{};
  bool collided = false;
  uint32_t retries = 0;
};

template <typename Archive>
void serialize(Archive& archive, Movement& movement)
{
  archive(movement.direction, movement.collided, movement.retries);
}
}  // namespace dl
