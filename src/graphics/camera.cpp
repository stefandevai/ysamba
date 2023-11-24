#include "./camera.hpp"

#include <cmath>

namespace dl
{
Camera::Camera() {}

const glm::ivec2 Camera::get_position_in_tiles() const
{
  assert(m_tile_size.x != 0.f && m_tile_size.y != 0.f && "Tile size was not set");

  return glm::ivec2{m_position.x / m_tile_size.x, m_position.y / m_tile_size.y};
}

void Camera::move(const float x, const float y, const float z)
{
  m_position.x += x;
  m_position.y += y;
  m_position.z += z;

  m_front.x += x;
  m_front.y += y;
  m_front.z += z;
}

void Camera::set_position(const float x, const float y, const float z)
{
  m_position.x = x;
  m_position.y = y;
  m_position.z = z;

  m_front.x = x;
  m_front.y = y;
  m_front.z = z - 501;
}

void Camera::set_size(const float width, const float height) { set_frustrum(0.f, width, height, 0.f); }

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

void Camera::set_tile_size(const float width, const float height)
{
  assert(width > 0 && "Tile width must be greather than 0");
  assert(height > 0 && "Tile height must be greather than 0");

  m_tile_size.x = width;
  m_tile_size.y = height;
  m_size_in_tiles.x = std::ceil(m_size.x / width);
  m_size_in_tiles.y = std::ceil(m_size.y / height);
}

}  // namespace dl
