#pragma once

#include <cmath>

namespace dl
{
struct Vector2i;
}

namespace dl::utils
{
// Iterpolates value between two ranges
template <typename T>
static T interpolate(const T start_1, const T end_1, const T start_2, const T end_2, const T value)
{
  return std::lerp(start_2, end_2, (value - start_1) / (end_1 - start_1));
}

static constexpr int array_index(int x, int y, int width)
{
  return y * width + x;
}

bool point_aabb(const Vector2i& point, const Vector2i& rect_position, const Vector2i& rect_size);
}  // namespace dl::utils
