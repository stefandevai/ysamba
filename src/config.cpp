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
std::string item_data = "data/items/items.json";
}  // namespace path

namespace display
{
int width = 1024;
int height = 576;
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
    if (path.contains("item_data"))
    {
      path::item_data = path.at("item_data").get<std::string>();
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

    if (display.contains("width"))
    {
      display::width = display.at("width").get<int>();
    }

    if (display.contains("height"))
    {
      display::height = display.at("height").get<int>();
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
}
}  // namespace dl::config
