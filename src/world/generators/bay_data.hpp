#pragma once

#include "../point.hpp"

namespace dl
{
struct BayData
{
  BayData(const int area, const Point<int>& point) : area(area), point(point) {}
  int area = 0;
  Point<int> point = Point<int>(0, 0);
};
}  // namespace dl
