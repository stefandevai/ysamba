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

    if (path.contains("assets"))
    {
      path::assets = path.at("assets").get<std::string>();
    }
    if (path.contains("translations"))
    {
      path::translations = path.at("translations").get<std::string>();
    }
    if (path.contains("tile_data"))
    {
      path::tile_data = path.at("tile_data").get<std::string>();
    }
    if (path.contains("tile_rules"))
    {
      path::tile_rules = path.at("tile_rules").get<std::string>();
    }
    if (path.contains("item_data"))
    {
      path::item_data = path.at("item_data").get<std::string>();
    }
    if (path.contains("input"))
    {
      path::input = path.at("input").get<std::string>();
    }
  }

  if (json.object.contains("world"))
  {
    auto& world = json.object.at("world");

    if (world.contains("texture_id"))
    {
      world::texture_id = world.at("texture_id").get<std::string>();
    }

    if (world.contains("spatial_hash_cell_size"))
    {
      world::spatial_hash_cell_size = world.at("spatial_hash_cell_size").get<uint32_t>();
    }
  }

  if (json.object.contains("display"))
  {
    auto& display = json.object.at("display");

    if (display.contains("default_width"))
    {
      display::default_width = display.at("default_width").get<int>();
    }

    if (display.contains("default_height"))
    {
      display::default_height = display.at("default_height").get<int>();
    }

    if (display.contains("title"))
    {
      display::title = display.at("title").get<std::string>();
    }
  }

  if (json.object.contains("pathfinding"))
  {
    auto& pathfinding = json.object.at("pathfinding");

    if (pathfinding.contains("max_search_radius"))
    {
      pathfinding::search_radius = pathfinding.at("max_search_radius").get<uint32_t>();
    }

    if (pathfinding.contains("max_tries_after_collision"))
    {
      pathfinding::tries_after_collision = pathfinding.at("max_tries_after_collision").get<uint32_t>();
    }
  }

  if (json.object.contains("gameplay"))
  {
    auto& gameplay = json.object.at("gameplay");

    if (gameplay.contains("default_zoom"))
    {
      gameplay::default_zoom = gameplay.at("default_zoom").get<double>();
    }
  }

  if (json.object.contains("world_creation"))
  {
    auto& world_creation = json.object.at("world_creation");

    if (world_creation.contains("world_width"))
    {
      world_creation::world_width = world_creation.at("world_width").get<uint32_t>();
    }
    if (world_creation.contains("world_height"))
    {
      world_creation::world_height = world_creation.at("world_height").get<uint32_t>();
    }
    if (world_creation.contains("world_depth"))
    {
      world_creation::world_depth = world_creation.at("world_depth").get<uint32_t>();
    }
  }
}
}  // namespace dl::config
