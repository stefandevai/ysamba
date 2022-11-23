#include "./tilemap.hpp"

#include <algorithm>

namespace dl
{
  Tilemap::Tilemap() { }

  Tilemap::Tilemap(std::vector<int> tiles, const int width, const int height)
    : tiles(tiles), m_width(width), m_height(height)
  { }

  int Tilemap::at(const int x, const int y) const
  {
    if (x >= m_width || x < 0 || y >= m_height || y < 0)
    {
      return m_null_tile;
    }

    return tiles[m_width * y + x];
  }

  int Tilemap::at(const Point<int>& point) const
  {
    if (point.x >= m_width || point.x < 0 || point.y >= m_height || point.y < 0)
    {
      return m_null_tile;
    }

    return tiles[m_width * point.y + point.x];
  }

  Size Tilemap::get_size() const
  {
    return Size{m_width, m_height, 0};
  }
}
