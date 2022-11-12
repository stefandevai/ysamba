#include "./point.hpp"

namespace dl
{
  Point::Point(const unsigned int x, const unsigned int y)
    : x(x), y(y)
  {}

  Point Point::left() const
  {
    return Point(x - 1, y);
  }

  Point Point::right() const
  {
    return Point(x + 1, y);
  }

  Point Point::bottom() const
  {
    return Point(x, y - 1);
  }

  Point Point::top() const
  {
    return Point(x, y + 1);
  }

  Point Point::top_left() const
  {
    return Point(x - 1, y + 1);
  }

  Point Point::top_right() const
  {
    return Point(x + 1, y + 1);
  }

  Point Point::bottom_right() const
  {
    return Point(x + 1, y - 1);
  }

  Point Point::bottom_left() const
  {
    return Point(x - 1, y - 1);
  }
}
