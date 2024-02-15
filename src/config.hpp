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
extern std::string action_data;
extern std::string tile_rules;
extern std::string item_data;
extern std::string input;
}  // namespace path

namespace display
{
extern int default_width;
extern int default_height;
extern std::string title;
}  // namespace display

namespace world
{
extern std::string texture_id;
extern uint32_t spatial_hash_cell_size;
}  // namespace world

namespace pathfinding
{
extern uint32_t max_steps;
extern uint32_t tries_after_collision;
}  // namespace pathfinding

namespace gameplay
{
extern double default_zoom;
}

namespace world_creation
{
extern uint32_t world_width;
extern uint32_t world_height;
extern uint32_t world_depth;
}  // namespace world_creation

void load(const std::filesystem::path& filepath = "data/config.json");
}  // namespace dl::config
