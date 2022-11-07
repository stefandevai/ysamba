#include "./tilemap.hpp"
#include <algorithm>
#include <sol/sol.hpp> // IWYU pragma: export

namespace dl
{
  Tilemap::Tilemap() { }

  Tilemap::Tilemap(std::vector<int> tiles, const int width, const int height)
    : m_tiles(tiles), m_width(width), m_height(height)
  { }

  int Tilemap::get(const int x, const int y) const
  {
    if (x >= m_width || x < 0 || y >= m_height || y < 0)
    {
      return m_null_tile;
    }

    return m_tiles[m_width * y + x];
  }

  Size Tilemap::get_size() const
  {
    return Size{m_width, m_height, 0};
  }
}
