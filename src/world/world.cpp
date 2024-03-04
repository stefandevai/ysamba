#include "./world.hpp"

#include <spdlog/spdlog.h>

#include <entt/core/hashed_string.hpp>
#include <entt/entity/registry.hpp>
#include <fstream>
#include <libtcod.hpp>
#include <nlohmann/json.hpp>
#include <queue>
#include <set>

#include "./a_star.hpp"
#include "./cell.hpp"
#include "./generators/game_chunk_generator.hpp"
#include "./generators/tile_rules.hpp"
#include "./item_factory.hpp"
#include "./society/job_type.hpp"
#include "./society/society_generator.hpp"
#include "./tile_flag.hpp"
#include "config.hpp"
#include "constants.hpp"
#include "core/game_context.hpp"
#include "core/input_manager.hpp"
#include "core/json.hpp"
#include "core/serialization.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/sprite.hpp"
#include "graphics/camera.hpp"
#include "graphics/display.hpp"
#include "graphics/renderer/texture.hpp"

namespace dl
{
World::World(GameContext& game_context) : m_game_context(game_context)
{
  m_texture_id = entt::hashed_string::value(config::world::texture_id.c_str());
  spatial_hash.load(config::world::spatial_hash_cell_size);

  spdlog::debug("Loading world \"{}\"", m_game_context.world_metadata.name);
  spdlog::debug("Seed: {}", m_game_context.world_metadata.seed);
  spdlog::debug("Id: {}", m_game_context.world_metadata.id);

  TileRules::load();
  m_load_tile_data();
  m_load_item_data();
}

void World::initialize(entt::registry& registry, const Camera& camera)
{
  using namespace entt::literals;
  auto society_blueprint = get_society("otomi"_hs);
  auto components = SocietyGenerator::generate_members(society_blueprint);
  SocietyGenerator::place_members(components, *this, camera, registry);

  has_initialized = true;
}

void World::generate_societies()
{
  auto society = SocietyGenerator::generate_blueprint();
  m_societies[society.id] = std::move(society);
}

void World::set_terrain(const uint32_t tile_id, const int x, const int y, const int z)
{
  auto& chunk = chunk_manager.at(x, y, z);
  chunk.tiles.set(
      tile_id, std::abs(x - chunk.position.x), std::abs(y - chunk.position.y), std::abs(z - chunk.position.z));

  if (!chunk.tiles.has_flags(DL_CELL_FLAG_VISIBLE, x, y, z))
  {
    chunk.tiles.compute_visibility();
  }
}

void World::set_decoration(const uint32_t tile_id, const int x, const int y, const int z)
{
  auto& chunk = chunk_manager.at(x, y, z);
  chunk.tiles.set_decoration(
      tile_id, std::abs(x - chunk.position.x), std::abs(y - chunk.position.y), std::abs(z - chunk.position.z));

  if (!chunk.tiles.has_flags(DL_CELL_FLAG_VISIBLE, x, y, z))
  {
    chunk.tiles.compute_visibility();
  }
}

void World::replace(const uint32_t from, const uint32_t to, const int x, const int y, const int z)
{
  const auto& cell = cell_at(x, y, z);

  if (cell.decoration == from && cell.terrain != to)
  {
    set_decoration(to, x, y, z);
    return;
  }
  // If the terrain already has the target tile, just assign the null tile to the over tile */
  else if (cell.decoration == from && cell.terrain == to)
  {
    set_decoration(0, x, y, z);
    return;
  }

  if (cell.terrain == from)
  {
    set_terrain(to, x, y, z);
  }
}

const Cell& World::cell_at(const int x, const int y, const int z) const
{
  const auto& chunk = chunk_manager.at(x, y, z);
  return chunk.tiles.cell_at(
      std::abs(x - chunk.position.x), std::abs(y - chunk.position.y), std::abs(z - chunk.position.z));
}

uint32_t World::terrain_at(const int x, const int y, const int z) const
{
  const auto& chunk = chunk_manager.at(x, y, z);
  return chunk.tiles.terrain_at(
      std::abs(x - chunk.position.x), std::abs(y - chunk.position.y), std::abs(z - chunk.position.z));
}

uint32_t World::decoration_at(const int x, const int y, const int z) const
{
  const auto& chunk = chunk_manager.at(x, y, z);
  return chunk.tiles.decoration_at(
      std::abs(x - chunk.position.x), std::abs(y - chunk.position.y), std::abs(z - chunk.position.z));
}

const TileData& World::get(const int x, const int y, const int z) const
{
  const auto& cell = cell_at(x, y, z);

  if (cell.decoration != 0)
  {
    return tile_data.at(cell.decoration);
  }

  return tile_data.at(cell.terrain);
}

const WorldTile World::get_all(const int x, const int y, const int z) const
{
  const auto& cell = cell_at(x, y, z);
  return WorldTile{tile_data.at(cell.terrain), tile_data.at(cell.decoration)};
}

const TileData& World::get_terrain(const int x, const int y, const int z) const
{
  const auto tile_index = terrain_at(x, y, z);
  return tile_data.at(tile_index);
}

const TileData& World::get_decoration(const int x, const int y, const int z) const
{
  const auto over_tile_index = decoration_at(x, y, z);
  return tile_data.at(over_tile_index);
}

int World::get_elevation(const int x, const int y) const
{
  auto& chunk = chunk_manager.at(x, y, 0);
  const uint32_t index = std::abs(x - chunk.position.x) + std::abs(y - chunk.position.y) * chunk.tiles.size.x;

  if (chunk.tiles.height_map.size() <= index)
  {
    return 0;
  }

  return chunk.tiles.height_map[index];
}

Vector3i World::screen_to_world(const Vector2i& position, const Camera& camera) const
{
  const auto& camera_position = camera.get_position();
  const auto& grid_size = camera.get_grid_size();

  auto world_position = Vector3i{std::floor((position.x + camera_position.x) / static_cast<double>(grid_size.x)),
                                 std::floor((position.y + camera_position.y) / static_cast<double>(grid_size.y)),
                                 0.0};

  for (int z = world::chunk_size.z - 1; z >= 0; --z)
  {
    const int queried_elevation = get_elevation(world_position.x, world_position.y + z);

    if (queried_elevation == z)
    {
      world_position.z = queried_elevation;
      world_position.y += queried_elevation;
      break;
    }
  }

  return world_position;
}

Vector3i World::mouse_to_world(const Camera& camera) const
{
  const auto& mouse_position = InputManager::get_instance().get_mouse_position();
  return screen_to_world(mouse_position, camera);
}

std::vector<Vector3i> World::find_path(const Vector3i& from, const Vector3i& to)
{
  // If tile is adjacent, just return a single step and avoid A* altogether
  if (std::abs(from.x - to.x) <= 1 && std::abs(from.y - to.y) <= 1 && std::abs(from.z - to.z) <= 1)
  {
    return {to};
  }

  AStar a_star(*this, from, to);

  do
  {
    a_star.step();
  } while (a_star.state == AStar::State::SEARCHING);

  // a_star.debug(*m_game_context.registry, true, false);

  if (a_star.state == AStar::State::SUCCEEDED)
  {
    return a_star.path;
  }

  return {};
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

    for (const auto x_displacement : displacements)
    {
      for (const auto y_displacement : displacements)
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
  using namespace entt::literals;

  const auto& tile = get(x, y, z);
  bool walkable = tile.flags.contains(tile_flag::walkable);

  if (walkable)
  {
    assert(m_game_context.registry != nullptr);
    auto entity = spatial_hash.get_by_component<entt::tag<"collidable"_hs>>(x, y, z, *m_game_context.registry);
    if (m_game_context.registry->valid(entity))
    {
      walkable = false;
    }
  }

  return walkable;
}

bool World::is_empty(const int x, const int y, const int z) const
{
  const auto& cell = cell_at(x, y, z);
  return cell.terrain == 0 && cell.decoration == 0;
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

      const auto& cell = cell_at(position.x + i, position.y + j, position.z);

      if (cell.decoration == pattern_value)
      {
        continue;
      }

      if (cell.terrain != pattern_value)
      {
        return false;
      }
    }
  }

  return found_pattern;
}

const TileData& World::get_tile_data(const uint32_t id) const { return tile_data.at(id); }

const ItemData& World::get_item_data(const uint32_t id) const { return item_data.at(id); }

entt::entity World::create_item(
    entt::registry& registry, const uint32_t id, const int x, const int y, const int z) const
{
  const auto item = item_factory::create(get_item_data(id), registry);

  const auto render_data = Sprite{
      .id = id,
      .resource_id = m_texture_id,
      .category = "item",
      .layer_z = 1,
  };
  registry.emplace<Sprite>(item, render_data);
  registry.emplace<Position>(item, static_cast<double>(x), static_cast<double>(y), static_cast<double>(z));
  return item;
}

void World::m_load_tile_data()
{
  JSON json_tile_data{config::path::tile_data};

  const auto actions = m_load_actions();

  const auto texture = m_game_context.asset_manager->get<Texture>(m_texture_id);

  assert(texture != nullptr && "World texture is not loaded in order to get tile size");

  m_tile_size = texture->get_frame_size();

  assert(json_tile_data.object.contains("tiles") && "Tile data must contain a tiles array");

  const auto tiles = json_tile_data.object["tiles"].get<std::vector<nlohmann::json>>();

  for (const auto& tile : tiles)
  {
    auto tile_data = TileData();

    assert(tile.contains("id") && "Tile must have an id");
    assert(tile.contains("name") && "Tile must have a name");

    tile_data.id = tile["id"].get<uint32_t>();
    tile_data.name = tile["name"].get<std::string>();

    json::assign_if_contains<std::unordered_set<std::string>>(tile, "flags", tile_data.flags);
    json::assign_if_contains<Direction>(tile, "climbs_to", tile_data.climbs_to);

    if (tile.contains("actions"))
    {
      const auto actions_data = tile["actions"].get<std::vector<nlohmann::json>>();

      for (const auto& action_data : actions_data)
      {
        const auto id = action_data["id"].get<uint32_t>();

        assert(actions.contains(id) && "Action not found in the action data");

        auto action = actions.at(id);
        tile_data.actions[action.type] = std::move(action);
      }
    }

    this->tile_data[tile_data.id] = tile_data;
  }
}

std::unordered_map<uint32_t, Action> World::m_load_actions()
{
  JSON json_actions{config::path::action_data};
  std::unordered_map<uint32_t, Action> actions{};

  for (const auto& json_action : json_actions.object)
  {
    Action action{};

    assert(json_action.contains("id") && "Action must have an id");
    assert(json_action.contains("type") && "Action must have a type");

    action.id = json_action["id"].get<uint32_t>();
    action.type = json_action["type"].get<JobType>();

    json::assign_if_contains<std::string>(json_action, "label", action.label);
    json::assign_if_contains<uint32_t>(json_action, "turns_into", action.turns_into);
    json::assign_if_contains<std::vector<std::string>>(json_action, "qualities_required", action.qualities_required);

    if (json_action.contains("consumes"))
    {
      for (const auto& item : json_action["consumes"])
      {
        assert(item.contains("id") && "Consumed item must have an id");
        assert(item.contains("quantity") && "Consumed item must have a quantity");

        const auto item_id = item["id"].get<uint32_t>();
        action.consumes[item_id] = item["quantity"].get<uint32_t>();
      }
    }

    if (json_action.contains("gives"))
    {
      for (const auto& item : json_action["gives"])
      {
        assert(item.contains("item_id") && "Consumed item must have an item_id");
        assert(item.contains("quantity") && "Consumed item must have a quantity");

        const auto item_id = item["item_id"].get<uint32_t>();
        const auto quantity = item["quantity"].get<std::pair<uint32_t, uint32_t>>();
        action.gives[item_id] = quantity;
      }

      json::assign_if_contains<bool>(json_action, "gives_in_place", action.gives_in_place);
    }

    actions[action.id] = std::move(action);
  }

  return actions;
}

void World::m_load_item_data()
{
  JSON item_data_json{config::path::item_data};

  const auto items = item_data_json.object.get<std::vector<nlohmann::json>>();

  for (const auto& item : items)
  {
    auto item_data = ItemData();

    assert(item.contains("id") && "Item must have an id");
    assert(item.contains("name") && "Item must have a name");

    item_data.id = item["id"].get<uint32_t>();
    item_data.name = item["name"].get<std::string>();

    if (item.contains("weight"))
    {
      const std::string& weight_string = item["weight"].get<std::string>();
      const double weight = item_factory::parse_weight(weight_string);
      item_data.weight = weight;
      item_data.weight_string = std::move(weight_string);
    }
    if (item.contains("volume"))
    {
      const std::string& volume_string = item["volume"].get<std::string>();
      const double volume = item_factory::parse_volume(volume_string);
      item_data.volume = volume;
      item_data.volume_string = std::move(volume_string);
    }

    if (item.contains("qualities"))
    {
      for (const auto& quality : item["qualities"])
      {
        assert(quality.contains("name") && "Quality must have a name");
        assert(quality.contains("level") && "Quality must have a level");

        item_data.qualities[quality["name"].get<std::string>()] = quality["level"].get<int>();
      }
    }
    json::assign_if_contains<std::unordered_set<std::string>>(item, "flags", item_data.flags);
    json::assign_if_contains<std::vector<uint32_t>>(item, "weared_on", item_data.weared_on);

    if (item.contains("container"))
    {
      const auto& container = item["container"];

      assert(container.contains("materials") && "Container must have materials");
      assert(container.contains("weight_capacity") && "Container must have weight capacity");
      assert(container.contains("volume_capacity") && "Container must have volume capacity");

      json::assign_if_contains<std::vector<uint32_t>>(container, "materials", item_data.container.materials);

      if (container.contains("weight_capacity"))
      {
        item_data.container.weight_capacity
            = item_factory::parse_weight(item["container"]["weight_capacity"].get<std::string>());
      }

      if (container.contains("volume_capacity"))
      {
        item_data.container.volume_capacity
            = item_factory::parse_volume(item["container"]["volume_capacity"].get<std::string>());
      }
    }

    this->item_data[item_data.id] = item_data;
  }
}
}  // namespace dl
