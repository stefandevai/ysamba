#include "./world.hpp"

#include <spdlog/spdlog.h>

#include <nlohmann/json.hpp>
#include <queue>
#include <set>
/* #include "./generators/terrain_generator.hpp" */
#include "../graphics/renderer.hpp"
#include "../graphics/sprite.hpp"
#include "./generators/dummy_generator.hpp"
#include "./generators/society_generator.hpp"
#include "./tile_flag.hpp"
#include "./tile_target.hpp"

namespace dl
{
World::World()
{
  m_chunk_size = m_json.object["chunk_size"];
  m_texture_id = m_json.object["texture_id"];
  const auto spatial_hash_cell_size = m_json.object["spatial_hash_cell_size"];
  spatial_hash.load(m_chunk_size, m_chunk_size, spatial_hash_cell_size);

  m_load_tile_data();
  m_load_item_data();
}

void World::generate(const int width, const int height, const int seed)
{
  m_seed = seed;
  m_tilemaps.clear();

  /* auto tilemap_generator = TerrainGenerator(width, height); */
  auto tilemap_generator = DummyGenerator(width, height);
  auto tilemap = tilemap_generator.generate(seed);
  m_tilemaps.push_back(tilemap);

  /* auto society_generator = SocietyGenerator(); */
  auto society = SocietyGenerator::generate_blueprint();
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
  std::map<std::pair<int, int>, std::pair<int, int>> paths;
  TileTarget tile_target;
  std::queue<std::pair<int, int>> position_queue;
  std::set<std::pair<int, int>> visited;
  const auto displacements = {-1, 0, 1};

  bool found_tile = false;

  position_queue.push({x, y});
  visited.insert({x, y});

  while (!position_queue.empty() && !found_tile)
  {
    const auto [center_x, center_y] = position_queue.front();
    position_queue.pop();

    for (const auto& x_displacement : displacements)
    {
      for (const auto& y_displacement : displacements)
      {
        if (x_displacement == 0 && y_displacement == 0)
        {
          continue;
        }

        const auto current_x = center_x + x_displacement;
        const auto current_y = center_y + y_displacement;

        if (visited.contains({current_x, current_y}))
        {
          continue;
        }

        const auto position = std::make_pair(current_x, current_y);

        paths[position] = std::make_pair(center_x, center_y);
        visited.insert(position);

        const auto& tile = get(current_x, current_y, z);

        if (tile.flags.contains(flag))
        {
          tile_target.id = tile.id;
          tile_target.x = current_x;
          tile_target.y = current_y;
          tile_target.z = z;
          found_tile = true;

          const auto start = std::make_pair(x, y);
          auto step = position;

          while (step != start)
          {
            step = paths[step];
            tile_target.path.push(step);
          }
          break;
        }

        if (tile.flags.contains(tile_flag::walkable))
        {
          position_queue.push(position);
        }
      }

      if (found_tile)
      {
        break;
      }
    }
  }

  return tile_target;
}

bool World::adjacent(const int tile_id, const int x, const int y, const int z) const
{
  const auto displacements = {-1, 0, 1};

  for (const auto& x_displacement : displacements)
  {
    for (const auto& y_displacement : displacements)
    {
      if (x_displacement == 0 && y_displacement == 0)
      {
        continue;
      }

      const auto& tile = get(x + x_displacement, y + y_displacement, z);

      if (tile.id == tile_id)
      {
        return true;
      }
    }
  }

  return false;
}

TilemapSize World::get_tilemap_size(const int z) { return m_tilemaps[z - m_depth_min].get_size(); }

const TileData& World::get_tile_data(const uint32_t id) const { return m_tile_data.at(id); }

void World::m_load_tile_data()
{
  const auto tiles = m_json.object["tiles"].get<std::vector<nlohmann::json>>();

  for (const auto& tile : tiles)
  {
    auto tile_data = TileData();

    tile_data.id = tile["id"].get<int>();
    tile_data.name = tile["name"].get<std::string>();

    if (tile.contains("flags"))
    {
      tile_data.flags = tile["flags"].get<std::unordered_set<std::string>>();
    }
    if (tile.contains("drop_ids"))
    {
      tile_data.drop_ids = tile["drop_ids"].get<std::vector<uint32_t>>();
    }
    if (tile.contains("after_removed"))
    {
      tile_data.after_removed = tile["after_removed"].get<uint32_t>();
    }

    m_tile_data[tile_data.id] = tile_data;
  }
}

void World::m_load_item_data()
{
  m_json.load("./data/items/cooking.json");

  const auto items = m_json.object.get<std::vector<nlohmann::json>>();

  for (const auto& item : items)
  {
    auto item_data = ItemData();

    item_data.id = item["id"].get<uint32_t>();
    item_data.name = item["name"].get<std::string>();
    m_item_data[item_data.id] = item_data;
  }
}
}  // namespace dl
