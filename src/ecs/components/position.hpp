#pragma once

#include <cstdint>

#include "core/maths/vector.hpp"

namespace dl
{
struct Position : public Vector3
{
  uint32_t spatial_hash_index = 99999;
};
}  // namespace dl
