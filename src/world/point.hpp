#pragma once

#include <tuple>

namespace dl
{
template <typename T = int>
class Point
{
 public:
  T x;
  T y;

  Point(const T x, const T y) : x(x), y(y) {}
  Point() : x(0), y(0) {}

  Point(const Point& rhs)
  {
    x = rhs.x;
    y = rhs.y;
  }

  Point& operator=(const Point& b)
  {
    x = b.x;
    y = b.y;
    return *this;
  }

  bool operator==(const Point& b) const { return (x == b.x && y == b.y); }

  bool operator!=(const Point& b) const { return (x != b.x || y != b.y); }

  bool operator<(Point const& p) const { return std::tie(x, y) < std::tie(p.x, p.y); }

  Point left() const { return Point(x - 1, y); }

  Point right() const { return Point(x + 1, y); }

  Point bottom() const { return Point(x, y - 1); }

  Point top() const { return Point(x, y + 1); }

  Point top_left() const { return Point(x - 1, y + 1); }

  Point top_right() const { return Point(x + 1, y + 1); }

  Point bottom_right() const { return Point(x + 1, y - 1); }

  Point bottom_left() const { return Point(x - 1, y - 1); }

  constexpr int to_index(const int width) const { return y * width + x; }
};
}  // namespace dl
