#include "./world.hpp"

#include <spdlog/spdlog.h>

#include <libtcod.hpp>
#include <nlohmann/json.hpp>
#include <queue>
#include <set>
/* #include "./generators/terrain_generator.hpp" */
#include "./generators/dummy_generator.hpp"
#include "./society/society_generator.hpp"
#include "./tile_flag.hpp"
#include "./tile_target.hpp"
#include "core/game_context.hpp"
#include "graphics/sprite.hpp"
#include "graphics/texture.hpp"

namespace dl
{
World::World(GameContext& game_context) : m_game_context(game_context)
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
  m_terrains.clear();

  /* auto tilemap_generator = TerrainGenerator(width, height); */
  auto tilemap_generator = DummyGenerator(width, height);
  auto tilemap = tilemap_generator.generate(seed);
  m_terrains.push_back(tilemap);

  auto society = SocietyGenerator::generate_blueprint();
  m_societies[society.id] = society;
}

void World::load(const std::string& filepath)
{
  auto society = SocietyGenerator::generate_blueprint();
  m_societies[society.id] = society;

  m_json.load(filepath);
  const auto& layers = m_json.object.get<std::vector<nlohmann::json>>();

  m_terrains.clear();
  m_terrains.resize(layers.size());

  m_over_terrains.clear();
  m_over_terrains.resize(layers.size());

  for (size_t i = 0; i < layers.size(); ++i)
  {
    const auto width = layers[i]["width"].get<uint32_t>();
    const auto height = layers[i]["width"].get<uint32_t>();
    const auto terrain_data = layers[i]["terrain"].get<std::vector<int>>();
    const auto over_terrain_data = layers[i]["over_terrain"].get<std::vector<int>>();

    m_terrains[i] = Tilemap(terrain_data, width, height);
    m_over_terrains[i] = Tilemap(over_terrain_data, width, height);
  }
}

void World::set_terrain(const int tile_id, const int x, const int y, const int z)
{
  m_terrains[z - m_depth_min].set(tile_id, x, y);
}

void World::set_over_terrain(const int tile_id, const int x, const int y, const int z)
{
  m_over_terrains[z - m_depth_min].set(tile_id, x, y);
}

void World::replace(const int from, const int to, const int x, const int y, const int z)
{
  const int over_tile_index = m_over_terrains[z - m_depth_min].at(x, y);
  const int terrain_index = m_terrains[z - m_depth_min].at(x, y);

  if (over_tile_index == from && terrain_index != to)
  {
    m_over_terrains[z - m_depth_min].set(to, x, y);
    return;
  }
  // If the terrain already has the target tile, just assign the null tile to the over tile
  else if (over_tile_index == from && terrain_index == to)
  {
    m_over_terrains[z - m_depth_min].set(0, x, y);
    return;
  }

  if (terrain_index == from)
  {
    m_terrains[z - m_depth_min].set(to, x, y);
  }
}

const TileData& World::get(const int x, const int y, const int z) const
{
  const int over_tile_index = m_over_terrains[z - m_depth_min].at(x, y);

  if (over_tile_index != 0)
  {
    return m_tile_data.at(over_tile_index);
  }

  const int terrain_index = m_terrains[z - m_depth_min].at(x, y);

  return m_tile_data.at(terrain_index);
}

const WorldTile World::get_all(const int x, const int y, const int z) const
{
  const auto terrain_id = m_terrains[z - m_depth_min].at(x, y);
  const auto over_terrain_id = m_over_terrains[z - m_depth_min].at(x, y);

  return WorldTile{m_tile_data.at(terrain_id), m_tile_data.at(over_terrain_id)};
}

std::stack<std::pair<int, int>> World::get_path_between(const Vector3i& from, const Vector3i& to)
{
  std::stack<std::pair<int, int>> path{};

  // TODO: Use A* algorithm instead
  TCOD_bresenham_data_t bresenham_data;
  int x = to.x;
  int y = to.y;

  TCOD_line_init_mt(x, y, from.x, from.y, &bresenham_data);

  do
  {
    if (x == from.x && y == from.y)
    {
      break;
    }

    path.push({x, y});
  } while (!TCOD_line_step_mt(&x, &y, &bresenham_data));

  return path;
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

bool World::is_walkable(const int x, const int y, const int z) const
{
  const auto& tile = get(x, y, z);
  return tile.flags.contains(tile_flag::walkable);
}

bool World::has_pattern(const std::vector<uint32_t>& pattern, const Vector2i& size, const Vector3i& position) const
{
  bool found_pattern = true;

  for (int j = 0; j < size.y; ++j)
  {
    for (int i = 0; i < size.x; ++i)
    {
      const auto pattern_value = pattern[j * size.x + i];

      if (pattern_value == 0)
      {
        continue;
      }

      const uint32_t over_terrain_index = m_over_terrains[position.z - m_depth_min].at(position.x + i, position.y + j);

      if (over_terrain_index == pattern_value)
      {
        continue;
      }

      const uint32_t terrain_index = m_terrains[position.z - m_depth_min].at(position.x + i, position.y + j);

      if (terrain_index != pattern_value)
      {
        return false;
      }
    }
  }

  return found_pattern;
}

TilemapSize World::get_tilemap_size(const int z) { return m_terrains[z - m_depth_min].get_size(); }

const TileData& World::get_tile_data(const uint32_t id) const { return m_tile_data.at(id); }

void World::m_load_tile_data()
{
  const auto& texture = m_game_context.asset_manager->get<Texture>(m_texture_id);
  assert(texture != nullptr && "World texture is not loaded in order to get tile size");

  m_tile_size.x = texture->get_frame_width();
  m_tile_size.y = texture->get_frame_height();

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
