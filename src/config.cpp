#include "config.hpp"

#include "core/json.hpp"

namespace dl::config
{
std::string initial_scene = "home_menu";

namespace path
{
std::string assets = "data/assets.json";
std::string translations = "data/translations";
std::string tile_data = "data/world/tiles.json";
std::string action_data = "data/world/actions.json";
std::string tile_rules = "data/world/tile_rules.json";
std::string item_data = "data/items/items.json";
std::string input = "data/input.json";
}  // namespace path

namespace display
{
int default_width = 1024;
int default_height = 576;
std::string title = "Colonization of Ysamba";
}  // namespace display

namespace world
{
std::string texture_id = "spritesheet-tileset";
uint32_t spatial_hash_cell_size = 2;
}  // namespace world

namespace pathfinding
{
uint32_t max_steps = 700;
uint32_t tries_after_collision = 3;
}  // namespace pathfinding

namespace ai
{
int default_job_priority = 2;
}

namespace gameplay
{
double default_zoom = 1.0;
}

namespace world_creation
{
uint32_t world_width = 256;
uint32_t world_height = 256;
uint32_t world_depth = 256;
}  // namespace world_creation

void load(const std::filesystem::path& filepath)
{
  if (!std::filesystem::exists(filepath))
  {
    return;
  }

  JSON json(filepath.string());

  if (json.object.contains("initial_scene"))
  {
    initial_scene = json.object.at("initial_scene").get<std::string>();
  }

  if (json.object.contains("path"))
  {
    auto& path = json.object.at("path");

    json::assign_if_contains<std::string>(path, "assets", path::assets);
    json::assign_if_contains<std::string>(path, "translations", path::translations);
    json::assign_if_contains<std::string>(path, "tile_data", path::tile_data);
    json::assign_if_contains<std::string>(path, "action_data", path::action_data);
    json::assign_if_contains<std::string>(path, "tile_rules", path::tile_rules);
    json::assign_if_contains<std::string>(path, "item_data", path::item_data);
    json::assign_if_contains<std::string>(path, "input", path::input);
  }

  if (json.object.contains("world"))
  {
    auto& world = json.object.at("world");

    json::assign_if_contains<std::string>(world, "texture_id", world::texture_id);
    json::assign_if_contains<uint32_t>(world, "spatial_hash_cell_size", world::spatial_hash_cell_size);
  }

  if (json.object.contains("display"))
  {
    auto& display = json.object.at("display");

    json::assign_if_contains<int>(display, "default_width", display::default_width);
    json::assign_if_contains<int>(display, "default_height", display::default_height);
    json::assign_if_contains<std::string>(display, "title", display::title);
  }

  if (json.object.contains("pathfinding"))
  {
    auto& pathfinding = json.object.at("pathfinding");

    json::assign_if_contains<uint32_t>(pathfinding, "max_steps", pathfinding::max_steps);
    json::assign_if_contains<uint32_t>(pathfinding, "tries_after_collision", pathfinding::tries_after_collision);
  }

  if (json.object.contains("ai"))
  {
    auto& ai = json.object.at("ai");

    json::assign_if_contains<int>(ai, "default_job_priority", ai::default_job_priority);
  }

  if (json.object.contains("gameplay"))
  {
    auto& gameplay = json.object.at("gameplay");

    json::assign_if_contains<double>(gameplay, "default_zoom", gameplay::default_zoom);
  }

  if (json.object.contains("world_creation"))
  {
    auto& world_creation = json.object.at("world_creation");

    json::assign_if_contains<uint32_t>(world_creation, "world_width", world_creation::world_width);
    json::assign_if_contains<uint32_t>(world_creation, "world_height", world_creation::world_height);
    json::assign_if_contains<uint32_t>(world_creation, "world_depth", world_creation::world_depth);
  }
}
}  // namespace dl::config
