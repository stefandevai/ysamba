#pragma once

#include <string>
#include <map>
#include <libtcod.hpp>
#include <cereal/types/vector.hpp>
#include "../core/lua_api.hpp"
#include "../components/size.hpp"
#include "./tile_data.hpp"

namespace dl
{
  class Tilemap
  {
    public:
      Tilemap();
      Tilemap(std::vector<int> tiles, const int width, const int height);

      int get(const int x, const int y) const;
      Size get_size() const;

      template<class Archive> 
      void serialize(Archive& archive)
      {
        archive(m_tiles, m_width, m_height, m_null_tile);
      }

    private:
      LuaAPI m_lua;
      std::vector<int> m_tiles;
      int m_width = 0;
      int m_height = 0;
      int m_null_tile = 0;
  };
}
