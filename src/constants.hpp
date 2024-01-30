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

namespace dl::world
{
const Vector3i chunk_size{128, 128, 10};
}  // namespace dl::world
