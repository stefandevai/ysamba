#pragma once

#include <cereal/types/vector.hpp>
#include "../components/size.hpp"
#include "./tile_data.hpp"
#include "./point.hpp"

namespace dl
{
  class Tilemap
  {
    public:
      std::vector<int> tiles;

      Tilemap();
      Tilemap(std::vector<int> tiles, const int width, const int height);

      int operator [](int i) const
      {
        return tiles[i];
      }

      int& operator [](int i)
      {
        return tiles[i];
      }

      int at(const int x, const int y) const;
      int at(const Point<int>& point) const;

      Size get_size() const;

      template<class Archive> 
      void serialize(Archive& archive)
      {
        archive(tiles, m_width, m_height, m_null_tile);
      }

    private:
      int m_width = 0;
      int m_height = 0;
      int m_null_tile = 0;
  };
}
