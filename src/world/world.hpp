#pragma once

#include <vector>
#include <map>
#include <libtcod.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/memory.hpp>
#include <entt/entity/registry.hpp>
#include "../core/lua_api.hpp"
#include "../components/size.hpp"
#include "./tile_data.hpp"
#include "./tilemap.hpp"
#include "./camera.hpp"
#include "./society.hpp"

namespace dl
{
  class World
  {
    public:
      World();

      void generate(const int width, const int height, const int seed);
      void update(const uint32_t delta);
      void render(TCOD_Console& console, const Camera& camera);
      const TileData get(const int x, const int y, const int z);
      Size get_tilemap_size(const int z);
      inline int get_seed() const { return m_seed; };
      inline Society get_society(const std::string& society_id) const { return m_societies.at(society_id); };

      template<class Archive> 
      void serialize(Archive& archive)
      {
        archive(m_depth_min, m_depth_max, m_tilemaps, m_seed, m_societies);
      }

    private:
      LuaAPI m_lua;
      /* std::vector<std::shared_ptr<Tilemap>> m_tilemaps; */
      std::vector<Tilemap> m_tilemaps;
      std::map<int, TileData> m_tile_data;
      int m_depth_min = 0;
      int m_depth_max = 1;
      int m_seed = 0;
      std::map<std::string, Society> m_societies;

      void m_load_tile_data();
  };
}
