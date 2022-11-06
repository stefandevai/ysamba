#include "./tilemap.hpp"
#include <algorithm>
#include <sol/sol.hpp> // IWYU pragma: export

// TEMP
#include <iostream>
// TEMP

namespace dl
{
  Tilemap::Tilemap()
  {
    m_load();
  }

  Tilemap::~Tilemap()
  {

  }

  void Tilemap::generate(const std::string& seed)
  {


  }

  void Tilemap::update(const uint32_t delta)
  {

  }

  void Tilemap::render(TCOD_Console& console)
  {
    const int screen_tiles_width = 40;
    const int screen_tiles_height = 25;

    const int local_width = std::min(screen_tiles_width, m_width);
    const int local_height = std::min(screen_tiles_height, m_height);


    for (int i = 0; i < local_width; ++i)
    {
      for (int j = 0; j < local_height; ++j)
      {
        const auto& tile = m_tile_data[m_map_chunk[m_width * j + i]];
        tcod::print(console, {i, j}, tile.symbol, TCOD_white, std::nullopt);
      }
    }
  }

  const TileData Tilemap::get(const int x, const int y, const int z)
  {
    if (x > m_width || x < 0 || y > m_height || y < 0)
    {
      return m_null_tile;
    }

    return m_tile_data[m_map_chunk[m_width * y + x]];
  }

  void Tilemap::m_load()
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

      m_tile_data[tile_data.id] = tile_data;
    }
  }
}
