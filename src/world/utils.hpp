#pragma once

#include "core/maths/vector.hpp"

namespace dl::utils
{
Vector2 world_to_map(const Vector3i& world_position);
Vector2i map_to_world(const Vector2i& map_position);
}  // namespace dl::utils

