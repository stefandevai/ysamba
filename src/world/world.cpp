#include "./world.hpp"

#include <spdlog/spdlog.h>

#include <entt/entity/registry.hpp>
#include <libtcod.hpp>
#include <nlohmann/json.hpp>
#include <queue>
#include <set>
/* #include "./generators/terrain_generator.hpp" */
/* #include "./generators/dummy_generator.hpp" */
#include "./generators/map_generator.hpp"
#include "./item_factory.hpp"
#include "./society/job_type.hpp"
#include "./society/society_generator.hpp"
#include "./tile_flag.hpp"
#include "core/game_context.hpp"
#include "ecs/components/position.hpp"
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

  tiles.set_size(width, height, 10);
  over_tiles.set_size(width, height, 10);

  auto map_generator = MapGenerator(tiles.size.x, tiles.size.y, tiles.size.z);
  map_generator.generate(m_seed);

  tiles.values = std::move(map_generator.tiles);
  tiles.height_map = std::move(map_generator.height_map);

  over_tiles.values.reserve(tiles.size.x * tiles.size.y * tiles.size.z);
  /* auto tilemap_generator = TerrainGenerator(width, height); */
  /* auto tilemap_generator = DummyGenerator(width, height); */

  auto society = SocietyGenerator::generate_blueprint();
  m_societies[society.id] = society;
}

void World::load(const std::string& filepath)
{
  auto society = SocietyGenerator::generate_blueprint();
  m_societies[society.id] = society;

  m_json.load(filepath);
  const auto& layers = m_json.object["layers"].get<std::vector<nlohmann::json>>();

  const auto width = m_json.object["width"].get<int>();
  const auto height = m_json.object["height"].get<int>();
  const auto depth = layers.size();

  tiles.set_size(width, height, depth);
  over_tiles.set_size(width, height, depth);

  tiles.values.clear();
  tiles.values.reserve(tiles.size.x * tiles.size.y * tiles.size.z);

  over_tiles.values.clear();
  over_tiles.values.reserve(tiles.size.x * tiles.size.y * tiles.size.z);

  tiles.height_map.clear();
  tiles.height_map.resize(tiles.size.x * tiles.size.y);

  for (std::size_t z = 0; z < layers.size(); ++z)
  {
    const auto terrain_data = layers[z]["terrain"].get<std::vector<uint32_t>>();
    const auto over_terrain_data = layers[z]["over_terrain"].get<std::vector<uint32_t>>();

    for (auto j = 0; j < tiles.size.y; ++j)
    {
      for (auto i = 0; i < tiles.size.x; ++i)
      {
        Cell cell{};
        cell.id = terrain_data[i + j * tiles.size.x];

        tiles.values[i + j * tiles.size.x + z * tiles.size.x * tiles.size.y] = std::move(cell);

        if (tiles.id_at(i, j, z) != 0)
        {
          tiles.height_map[i + j * tiles.size.x] =
              std::max(static_cast<int>(z), tiles.height_map[i + j * tiles.size.x]);
        }
      }
    }

    for (auto j = 0; j < tiles.size.y; ++j)
    {
      for (auto i = 0; i < tiles.size.x; ++i)
      {
        over_tiles.set(over_terrain_data[i + j * tiles.size.x], i, j, z);
      }
    }
  }
}

void World::set_terrain(const uint32_t tile_id, const int x, const int y, const int z) { tiles.set(tile_id, x, y, z); }

void World::set_over_terrain(const uint32_t tile_id, const int x, const int y, const int z)
{
  over_tiles.set(tile_id, x, y, z);
}

void World::replace(const uint32_t from, const uint32_t to, const int x, const int y, const int z)
{
  if (x < 0 || x >= tiles.size.x || y < 0 || y >= tiles.size.y || z < 0 || z >= tiles.size.z)
  {
    return;
  }

  const auto over_tile_index = over_tiles.id_at(x, y, z);
  const auto tile_index = tiles.id_at(x, y, z);

  if (over_tile_index == from && tile_index != to)
  {
    over_tiles.set(to, x, y, z);
    return;
  }
  // If the terrain already has the target tile, just assign the null tile to the over tile
  else if (over_tile_index == from && tile_index == to)
  {
    over_tiles.set(0, x, y, z);
    return;
  }

  if (tile_index == from)
  {
    tiles.set(to, x, y, z);
  }
}

const TileData& World::get(const int x, const int y, const int z) const
{
  if (x < 0 || x >= tiles.size.x || y < 0 || y >= tiles.size.y || z < 0 || z >= tiles.size.z)
  {
    return m_tile_data.at(0);
  }

  const int over_tile_index = over_tiles.id_at(x, y, z);

  if (over_tile_index != 0)
  {
    return m_tile_data.at(over_tile_index);
  }

  const int tile_index = tiles.id_at(x, y, z);

  return m_tile_data.at(tile_index);
}

const WorldTile World::get_all(const int x, const int y, const int z) const
{
  const auto over_tile_index = over_tiles.id_at(x, y, z);
  const auto tile_index = tiles.id_at(x, y, z);

  return WorldTile{m_tile_data.at(tile_index), m_tile_data.at(over_tile_index)};
}

const TileData& World::get_terrain(const int x, const int y, const int z) const
{
  const auto tile_index = tiles.id_at(x, y, z);
  return m_tile_data.at(tile_index);
}

const TileData& World::get_over_terrain(const int x, const int y, const int z) const
{
  const auto over_tile_index = over_tiles.id_at(x, y, z);
  return m_tile_data.at(over_tile_index);
}

int World::get_elevation(const int x, const int y) const
{
  if (x < 0 || x >= tiles.size.x || y < 0 || y >= tiles.size.y)
  {
    return 0;
  }

  return tiles.height_map[x + y * tiles.size.y];
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

bool World::adjacent(const uint32_t tile_id, const int x, const int y, const int z) const
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
  if (position.x < 0 || position.x >= tiles.size.x || position.y < 0 || position.y >= tiles.size.y || position.z < 0 ||
      position.z >= tiles.size.z)
  {
    return false;
  }

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

      const uint32_t over_tile_index = over_tiles.id_at(position.x + i, position.y + j, position.z);

      if (over_tile_index == pattern_value)
      {
        continue;
      }

      const uint32_t tile_index = tiles.id_at(position.x + i, position.y + j, position.z);

      if (tile_index != pattern_value)
      {
        return false;
      }
    }
  }

  return found_pattern;
}

const TileData& World::get_tile_data(const uint32_t id) const { return m_tile_data.at(id); }

const ItemData& World::get_item_data(const uint32_t id) const { return m_item_data.at(id); }

entt::entity World::create_item(
    entt::registry& registry, const uint32_t id, const int x, const int y, const int z) const
{
  const auto item = item_factory::create(get_item_data(id), registry);

  registry.emplace<Visibility>(item, get_texture_id(), id, "item", 1);
  registry.emplace<Position>(item, x, y, z);
  return item;
}

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

    tile_data.id = tile["id"].get<uint32_t>();
    tile_data.name = tile["name"].get<std::string>();

    if (tile.contains("flags"))
    {
      tile_data.flags = tile["flags"].get<std::unordered_set<std::string>>();
    }
    if (tile.contains("climbs_to"))
    {
      tile_data.climbs_to = tile["climbs_to"].get<Direction>();
    }
    if (tile.contains("actions"))
    {
      const auto actions = tile["actions"].get<std::vector<nlohmann::json>>();

      for (const auto& action : actions)
      {
        const auto& type = action["type"].get<JobType>();
        const auto& action_name = action["name"].get<std::string>();
        tile_data.actions[type].name = action_name;
        tile_data.actions[type].turns_into = action["turns_into"].get<uint32_t>();

        if (action.contains("qualities_required"))
        {
          tile_data.actions[type].qualities_required = action["qualities_required"].get<std::vector<std::string>>();
        }

        if (action.contains("consumes"))
        {
          for (const auto& item : action["consumes"])
          {
            tile_data.actions[type].consumes[item["id"].get<uint32_t>()] = item["quantity"].get<uint32_t>();
          }
        }

        if (action.contains("gives"))
        {
          for (const auto& item : action["gives"])
          {
            const auto item_id = item["item_id"].get<uint32_t>();
            const auto quantity = item["quantity"].get<std::pair<uint32_t, uint32_t>>();
            tile_data.actions[type].gives[item_id] = quantity;
          }

          tile_data.actions[type].gives_in_place = action["gives_in_place"].get<bool>();
        }
      }
    }

    m_tile_data[tile_data.id] = tile_data;
  }
}

void World::m_load_item_data()
{
  m_json.load("./data/items/items.json");

  const auto items = m_json.object.get<std::vector<nlohmann::json>>();

  for (const auto& item : items)
  {
    auto item_data = ItemData();

    item_data.id = item["id"].get<uint32_t>();
    item_data.name = item["name"].get<std::string>();

    const std::string& weight_string = item["weight"].get<std::string>();
    const double weight = item_factory::parse_weight(weight_string);
    item_data.weight = weight;
    item_data.weight_string = std::move(weight_string);

    const std::string& volume_string = item["volume"].get<std::string>();
    const double volume = item_factory::parse_volume(volume_string);
    item_data.volume = volume;
    item_data.volume_string = std::move(volume_string);

    if (item.contains("qualities"))
    {
      for (const auto& quality : item["qualities"])
      {
        item_data.qualities[quality["name"].get<std::string>()] = quality["level"].get<int>();
      }
    }
    if (item.contains("flags"))
    {
      item_data.flags = item["flags"].get<std::unordered_set<std::string>>();
    }
    if (item.contains("weared_on"))
    {
      item_data.weared_on = item["weared_on"].get<std::vector<uint32_t>>();
    }
    if (item.contains("container"))
    {
      item_data.container.materials = item["container"]["materials"].get<std::vector<uint32_t>>();
      item_data.container.weight_capacity =
          item_factory::parse_weight(item["container"]["weight_capacity"].get<std::string>());
      item_data.container.volume_capacity =
          item_factory::parse_volume(item["container"]["volume_capacity"].get<std::string>());
    }

    m_item_data[item_data.id] = item_data;
  }
}
}  // namespace dl
