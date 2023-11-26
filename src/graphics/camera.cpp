#include "./camera.hpp"

#include <cmath>

#include "core/display.hpp"

namespace dl
{
Camera::Camera() {}

Camera::Camera(const Display& display)
{
  const auto& display_size = display.get_size();
  set_size({static_cast<double>(display_size.x), static_cast<double>(display_size.y)});
}

const Vector2i Camera::get_position_in_tiles() const
{
  assert(m_tile_size.x != 0.f && m_tile_size.y != 0.f && "Tile size was not set");

  return Vector2i{m_position.x / m_tile_size.x, m_position.y / m_tile_size.y};
}

void Camera::move(const Vector3& quantity)
{
  m_position += quantity;
  m_front.x += quantity.x;
  m_front.y += quantity.y;
  m_front.z += quantity.z;
}

void Camera::set_position(const Vector3& position)
{
  m_position = position;

  m_front.x = position.x;
  m_front.y = position.y;
  m_front.z = position.z - 501;
}

void Camera::set_size(const Vector2& size)
{
  m_size = size;
  set_frustrum(0.f, size.x, size.y, 0.f);
}

void Camera::set_frustrum(const float left, const float right, const float bottom, const float top)
{
  m_frustrum_left = left;
  m_frustrum_right = right;
  m_frustrum_bottom = bottom;
  m_frustrum_top = top;

  m_size.x = std::abs(right - left);
  m_size.y = std::abs(top - bottom);

  if (m_tile_size.x > 0 && m_tile_size.y > 0)
  {
    m_size_in_tiles.x = std::ceil(m_size.x / m_tile_size.x);
    m_size_in_tiles.y = std::ceil(m_size.y / m_tile_size.y);
  }
}

void Camera::set_tile_size(const Vector2i& size)
{
  assert(size.x > 0 && "Tile width must be greather than 0");
  assert(size.y > 0 && "Tile height must be greather than 0");

  m_tile_size = size;
  m_size_in_tiles.x = std::ceil(m_size.x / size.x);
  m_size_in_tiles.y = std::ceil(m_size.y / size.y);
}

}  // namespace dl
