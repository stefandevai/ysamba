#pragma once

#include "world/point.hpp"

namespace dl
{
struct RiverSegment
{
  Point<double> point;
  Point<double> center;
  Point<double> normal;
  Point<double> tangent;
  double length = 0.0;
  std::shared_ptr<RiverSegment> previous;
  std::shared_ptr<RiverSegment> next;
};
}  // namespace dl
