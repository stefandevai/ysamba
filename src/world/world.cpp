#include "./world.hpp"

#include <nlohmann/json.hpp>
/* #include "./generators/terrain_generator.hpp" */
#include "./generators/dummy_generator.hpp"
#include "./generators/society_generator.hpp"
#include "../graphics/renderer.hpp"

namespace dl
{
  World::World()
  {
    m_load_tile_data();
  }

  void World::generate(const int width, const int height, const int seed)
  {
    m_seed = seed;
    m_tilemaps.clear();

    /* auto tilemap_generator = TerrainGenerator(width, height); */
    auto tilemap_generator = DummyGenerator(width, height);
    auto tilemap = tilemap_generator.generate(seed);
    m_tilemaps.push_back(tilemap);

    auto society_generator = SocietyGenerator();
    auto society = society_generator.generate(seed);
    m_societies[society.id] = society;
  }

  const TileData World::get(const int x, const int y, const int z)
  {
    const int tile_index = m_tilemaps[z - m_depth_min].at(x, y);

    return m_tile_data[tile_index];
  }

  Size World::get_tilemap_size(const int z)
  {
    return m_tilemaps[z - m_depth_min].get_size();
  }

  void World::m_load_tile_data()
  {
    m_json.load("./data/tilemap/tiles.json");
    const auto tiles = m_json.object["tiles"].get<std::vector<nlohmann::json>>();

    for (const auto& tile : tiles)
    {
      auto tile_data = TileData();

      tile_data.id = tile["id"].get<int>();
      tile_data.name = tile["name"].get<std::string>();
      tile_data.flags = tile["flags"].get<std::unordered_set<std::string>>();
      m_tile_data[tile_data.id] = tile_data;
    }
  }
}
