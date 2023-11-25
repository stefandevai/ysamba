#pragma once

#include "world/tile_target.hpp"

namespace dl
{
struct ActionHarvest
{
  TileTarget target;
  double time_left = 0.05;
};
}  // namespace dl
