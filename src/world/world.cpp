#include "./world.hpp"

#include <nlohmann/json.hpp>
/* #include "./generators/terrain_generator.hpp" */
#include "../graphics/renderer.hpp"
#include "../graphics/sprite.hpp"
#include "./generators/dummy_generator.hpp"
#include "./generators/society_generator.hpp"
#include "./tile_target.hpp"

namespace dl
{
World::World()
{
  m_load_tile_data();
  m_chunk_size = m_json.object["chunk_size"];
  m_texture_id = m_json.object["texture_id"];
  const auto spatial_hash_cell_size = m_json.object["spatial_hash_cell_size"];
  spatial_hash.load(m_chunk_size, m_chunk_size, spatial_hash_cell_size);
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

void World::set(const int tile_id, const int x, const int y, const int z)
{
  m_tilemaps[z - m_depth_min].set(tile_id, x, y);
}

const TileData& World::get(const int x, const int y, const int z) const
{
  const int tile_index = m_tilemaps[z - m_depth_min].at(x, y);

  return m_tile_data.at(tile_index);
}

TileTarget World::search_by_flag(const std::string& flag, const int x, const int y, const int z) const
{
  TileTarget tile_target;

  const auto tile_has_flag = [this, &tile_target, &flag](const int l_x, const int l_y, const int l_z) {
    const auto& tile = get(l_x, l_y, l_z);

    if (tile.flags.contains(flag))
    {
      tile_target.id = tile.id;
      tile_target.x = l_x;
      tile_target.y = l_y;
      tile_target.z = l_z;
      return true;
    }

    return false;
  };

  if (tile_has_flag(x - 1, y, z))
  {
    return tile_target;
  }
  if (tile_has_flag(x - 1, y + 1, z))
  {
    return tile_target;
  }
  if (tile_has_flag(x, y + 1, z))
  {
    return tile_target;
  }
  if (tile_has_flag(x + 1, y + 1, z))
  {
    return tile_target;
  }
  if (tile_has_flag(x + 1, y, z))
  {
    return tile_target;
  }
  if (tile_has_flag(x + 1, y - 1, z))
  {
    return tile_target;
  }
  if (tile_has_flag(x, y - 1, z))
  {
    return tile_target;
  }
  if (tile_has_flag(x - 1, y - 1, z))
  {
    return tile_target;
  }

  return tile_target;
}

bool World::adjacent(const int tile_id, const int x, const int y, const int z) const
{
  const auto& left_tile = get(x - 1, y, z);
  if (left_tile.id == tile_id)
  {
    return true;
  }

  const auto& top_left_tile = get(x - 1, y + 1, z);
  if (top_left_tile.id == tile_id)
  {
    return true;
  }

  const auto& top_tile = get(x, y + 1, z);
  if (top_tile.id == tile_id)
  {
    return true;
  }

  const auto& top_right_tile = get(x + 1, y + 1, z);
  if (top_right_tile.id == tile_id)
  {
    return true;
  }

  const auto& right_tile = get(x + 1, y, z);
  if (right_tile.id == tile_id)
  {
    return true;
  }

  const auto& bottom_right_tile = get(x + 1, y - 1, z);
  if (bottom_right_tile.id == tile_id)
  {
    return true;
  }

  const auto& bottom_tile = get(x, y - 1, z);
  if (bottom_tile.id == tile_id)
  {
    return true;
  }

  const auto& bottom_left_tile = get(x - 1, y - 1, z);
  if (bottom_left_tile.id == tile_id)
  {
    return true;
  }

  return false;
}

TilemapSize World::get_tilemap_size(const int z) { return m_tilemaps[z - m_depth_min].get_size(); }

void World::m_load_tile_data()
{
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
}  // namespace dl
