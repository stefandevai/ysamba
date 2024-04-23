#include "./utils.hpp"

#include "constants.hpp"

namespace dl::utils
{
Vector2 world_to_map(const Vector3i& world_position)
{
  return Vector2{
      world_position.x / static_cast<double>(world::map_to_tiles),
      world_position.y / static_cast<double>(world::map_to_tiles),
  };
}

Vector2i map_to_world(const Vector2i& map_position)
{
  return map_position * world::map_to_tiles;
}
}  // namespace dl::utils
