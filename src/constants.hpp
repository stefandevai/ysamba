#pragma once

#include <filesystem>

#include "core/maths/vector.hpp"

namespace dl::renderer
{
static const uint32_t layer_z_offset_items = 2;
static const uint32_t layer_z_offset_characters = 3;
}  // namespace dl::renderer

namespace dl::directory
{
const std::filesystem::path data{"dl_data"};
const std::filesystem::path worlds{data / "worlds"};
const std::filesystem::path chunks{"chunks"};
}  // namespace dl::directory

namespace dl::filename
{
const std::filesystem::path game{"game.dl"};
const std::filesystem::path metadata{".metadata"};
}  // namespace dl::filename

namespace dl::world
{
const Vector3i chunk_size{64, 64, 64};

// Quantity of tiles per map texture pixel
const int map_to_tiles = 20;
}  // namespace dl::world

namespace dl::pathfinding
{
constexpr int diagonal_cost_penalty = 41;
}  // namespace dl::pathfinding
