#pragma once

#include <cstdint>
#include <filesystem>
#include <string>

namespace dl::config
{
extern std::string initial_scene;

namespace path
{
extern std::string assets;
extern std::string translations;
extern std::string tile_data;
extern std::string item_data;
}  // namespace path

namespace display
{
extern int width;
extern int height;
extern std::string title;
}  // namespace display

namespace world
{
extern std::string texture_id;
extern uint32_t spatial_hash_cell_size;
}  // namespace world

namespace pathfinding
{
extern uint32_t search_radius;
extern uint32_t tries_after_collision;
}  // namespace pathfinding

void load(const std::filesystem::path& filepath = "data/config.json");
}  // namespace dl::config
