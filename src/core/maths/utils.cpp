#include "./utils.hpp"

#include "core/maths/vector.hpp"

namespace dl::utils
{
bool point_aabb(const Vector2i& point, const Vector2i& rect_position, const Vector2i& rect_size)
{
  return (point.x >= rect_position.x && point.x < rect_position.x + rect_size.x && point.y >= rect_position.y
          && point.y < rect_position.y + rect_size.y);
}
}  // namespace dl::utils
