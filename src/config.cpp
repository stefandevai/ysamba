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
uint32_t search_radius = 100;
uint32_t tries_after_collision = 3;
}  // namespace pathfinding

namespace gameplay
{
double default_zoom = 1.0;
}

namespace world_creation
{
uint32_t world_width = 256;
uint32_t world_height = 256;
uint32_t world_depth = 30;
}  // namespace world_creation

template <typename T>
void assign_if_contains(const nlohmann::json& json, const std::string& key, T& value)
{
  if (json.contains(key))
  {
    value = json.at(key).get<T>();
  }
}

void load(const std::filesystem::path& filepath)
{
  if (!std::filesystem::exists(filepath))
  {
    return;
  }

  JSON json{filepath};

  if (json.object.contains("initial_scene"))
  {
    initial_scene = json.object.at("initial_scene").get<std::string>();
  }

  if (json.object.contains("path"))
  {
    auto& path = json.object.at("path");

    assign_if_contains<std::string>(path, "assets", path::assets);
    assign_if_contains<std::string>(path, "translations", path::translations);
    assign_if_contains<std::string>(path, "tile_data", path::tile_data);
    assign_if_contains<std::string>(path, "tile_rules", path::tile_rules);
    assign_if_contains<std::string>(path, "item_data", path::item_data);
    assign_if_contains<std::string>(path, "input", path::input);
  }

  if (json.object.contains("world"))
  {
    auto& world = json.object.at("world");

    assign_if_contains<std::string>(world, "texture_id", world::texture_id);
    assign_if_contains<uint32_t>(world, "spatial_hash_cell_size", world::spatial_hash_cell_size);
  }

  if (json.object.contains("display"))
  {
    auto& display = json.object.at("display");

    assign_if_contains<int>(display, "default_width", display::default_width);
    assign_if_contains<int>(display, "default_height", display::default_height);
    assign_if_contains<std::string>(display, "title", display::title);
  }

  if (json.object.contains("pathfinding"))
  {
    auto& pathfinding = json.object.at("pathfinding");

    assign_if_contains<uint32_t>(pathfinding, "search_radius", pathfinding::search_radius);
    assign_if_contains<uint32_t>(pathfinding, "tries_after_collision", pathfinding::tries_after_collision);
  }

  if (json.object.contains("gameplay"))
  {
    auto& gameplay = json.object.at("gameplay");

    assign_if_contains<double>(gameplay, "default_zoom", gameplay::default_zoom);
  }

  if (json.object.contains("world_creation"))
  {
    auto& world_creation = json.object.at("world_creation");

    assign_if_contains<uint32_t>(world_creation, "world_width", world_creation::world_width);
    assign_if_contains<uint32_t>(world_creation, "world_height", world_creation::world_height);
    assign_if_contains<uint32_t>(world_creation, "world_depth", world_creation::world_depth);
  }
}
}  // namespace dl::config
