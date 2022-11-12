#pragma once

namespace dl
{
  struct Point
  {
    unsigned int x;
    unsigned int y;

    Point(const unsigned int x, const unsigned int y);

    bool operator!=(const Point& b) const
    {
      return (x != b.x || y != b.y);
    }

    Point left() const;
    Point right() const;
    Point bottom() const;
    Point top() const;

    Point top_left() const;
    Point top_right() const;
    Point bottom_right() const;
    Point bottom_left() const;
  };
}
