#include "./world.hpp"

#include <random>

namespace dl
{
  World::World()
  {
    m_load_tile_data();
  }

  void World::generate(const int width, const int height)
  {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(1,3);

    for (auto d = m_depth_min; d < m_depth_max; ++ d)
    {
      std::vector<int> tiles(width * height);

      for (auto i = 0; i < width; ++i)
      {
        for (auto j = 0; j < height; ++j)
        {
          tiles[j*width + i] = dist(rng);
        }
      }

      auto tilemap = std::make_shared<Tilemap>(tiles, width, height);

      m_tilemaps.push_back(tilemap);
    }

    /* m_write("./world.dl"); */
  }

  /* void World::m_write(const std::string& file_path) */
  /* { */
  /*   std::ofstream output_stream(file_path); */
  /*   cereal::BinaryOutputArchive archive (output_stream); */
  /*   archive(CEREAL_NVP(m_tilemaps)); */
  /* } */


  /* void World::load(const std::string& key) */
  /* { */

  /* } */

  void World::update(const uint32_t delta)
  {

  }

  void World::render(TCOD_Console& console, const Camera& camera)
  {
    for (int i = 0; i < camera.size.w; ++i)
    {
      for (int j = 0; j < camera.size.h; ++j)
      {
        const auto& tile = get(camera.position.x + i, camera.position.y + j, camera.position.z);
        console.at(i, j).ch = tile.symbol;
        console.at(i, j).fg = { tile.r, tile.g, tile.b, tile.a };
      }
    }
  }

  const TileData World::get(const int x, const int y, const int z)
  {
    const int tile_index = m_tilemaps[z - m_depth_min]->get(x, y);

    return m_tile_data[tile_index];
  }

  Size World::get_tilemap_size(const int z)
  {
    return m_tilemaps[z - m_depth_min]->get_size();
  }

  void World::m_load_tile_data()
  {
    // Load tile data
    m_lua.load("tilemap/tiles.lua");
    const auto tiles = m_lua.get_variable<std::vector<sol::table>>("tiles");

    for (const auto& tile : tiles)
    {
      auto tile_data = TileData();

      tile_data.id = tile["id"];
      tile_data.name = tile["name"];
      tile_data.symbol = tile["symbol"];
      tile_data.walkable = tile["walkable"];
      tile_data.r = tile["r"];
      tile_data.g = tile["g"];
      tile_data.b = tile["b"];
      tile_data.a = tile["a"];

      m_tile_data[tile_data.id] = tile_data;
    }
  }
}

