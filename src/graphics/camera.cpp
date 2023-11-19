#include "./camera.hpp"

#include <cmath>

namespace dl
{
Camera::Camera() {}

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

void Camera::set_frustrum(const float left, const float right, const float bottom, const float top)
{
  m_dimensions.x = std::abs(right - left);
  m_dimensions.y = std::abs(top - bottom);
  m_frustrum_left = left;
  m_frustrum_right = right;
  m_frustrum_bottom = bottom;
  m_frustrum_top = top;
}

}  // namespace dl
