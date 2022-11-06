#pragma once

#include <vector>
#include <map>
#include "../core/lua_api.hpp"
#include "../components/size.hpp"
#include "./tile_data.hpp"
#include "./tilemap.hpp"
#include "./camera.hpp"

namespace dl
{
  class World
  {
    public:
      World();
      ~World();

      void generate(const int width, const int height);
      void load(const std::string& key);
      void update(const uint32_t delta);
      void render(TCOD_Console& console, const Camera& camera);
      const TileData get(const int x, const int y, const int z);
      Size get_tilemap_size(const int z);

    private:
      LuaAPI m_lua;
      std::vector<std::shared_ptr<Tilemap>> m_tilemaps;
      std::map<int, TileData> m_tile_data;
      const int m_depth_min = 0;
      const int m_depth_max = 1;

      void m_write(const std::vector<int>& tilemap);
      void m_load_tile_data();
  };
}
