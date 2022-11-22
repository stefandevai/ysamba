#pragma once

#include "../point.hpp"

namespace dl
{
  struct RiverSegment
  {
    Point<int> point;
    Point<double> center;
    Point<double> normal;
    double length = 0.0;
    std::shared_ptr<RiverSegment> previous;
    std::shared_ptr<RiverSegment> next;
  };
}
