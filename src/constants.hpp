#pragma once

#include <filesystem>

#include "core/maths/vector.hpp"

namespace dl::renderer
{
static constexpr uint32_t layer_z_offset_items = 2;
static constexpr uint32_t layer_z_offset_characters = 3;
}  // namespace dl::renderer

namespace dl::directory
{
static const std::filesystem::path data{"dl_data"};
static const std::filesystem::path worlds{data / "worlds"};
static const std::filesystem::path chunks{"chunks"};
}  // namespace dl::directory

namespace dl::filename
{
static const std::filesystem::path game{"game.dl"};
static const std::filesystem::path metadata{".metadata"};
}  // namespace dl::filename

namespace dl::world
{
static constexpr Vector3i chunk_size{64, 64, 64};

// Quantity of tiles per map texture pixel
static constexpr int map_to_tiles = 20;
}  // namespace dl::world

namespace dl::pathfinding
{
static constexpr int diagonal_cost_penalty = 41;
}  // namespace dl::pathfinding
