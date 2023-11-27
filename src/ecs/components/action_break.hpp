#pragma once

#include "world/tile_target.hpp"

namespace dl
{
struct ActionBreak
{
  TileTarget target;
  double time_left = 0.05;
};
}  // namespace dl
