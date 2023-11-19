#pragma once

#include <cstdint>

namespace dl
{
struct Position
{
  double x;
  double y;
  double z;
  uint32_t spatial_hash_index = 99999;
};
}  // namespace dl
