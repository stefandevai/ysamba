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

glm::mat4 Camera::get_view_matrix() const
{
  glm::vec3 direction{};
  direction.x = std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch));
  direction.y = std::sin(glm::radians(pitch));
  direction.z = std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch));
  glm::vec3 pos{m_position.x, m_position.y, m_position.z};

  /* auto view_matrix = glm::lookAt(pos, pos + glm::normalize(direction), m_up); */
  /* view_matrix = glm::translate(view_matrix, glm::vec3(m_size.x / 2, m_size.y / 2, 0.0)); */
  /* view_matrix = glm::scale(view_matrix, glm::vec3(1.0f, std::sqrt(2.0f), 1.0f)); */
  /* view_matrix = glm::translate(view_matrix, glm::vec3(-m_size.x / 2, -m_size.y / 2, 0.0)); */

  /* return view_matrix; */
  /* return glm::scale(glm::lookAt(pos, pos + glm::normalize(direction), m_up), */
  /*                   glm::vec3(1.0f, std::sqrt(2.0f), 1.0f)); */

  if (m_resize_view_matrix)
  {
    auto view_matrix = glm::lookAt(pos, pos + glm::normalize(direction), m_up);
    return glm::scale(view_matrix, glm::vec3(1.0f, std::sqrt(2.0f), std::sqrt(2.0f)));
  }

  return glm::lookAt(pos, pos + glm::normalize(direction), m_up);
}

const Vector2i Camera::get_position_in_tiles() const
{
  assert(m_tile_size.x != 0.f && m_tile_size.y != 0.f && "Tile size was not set");

  return Vector2i{position.x / m_tile_size.x, position.y / m_tile_size.y};
}

void Camera::move(const Vector3& quantity)
{
  position += quantity;
  m_position.x += quantity.x;
  m_position.y += quantity.y * std::sqrt(2.0f);
  m_position.z += quantity.z;
  m_front.x += quantity.x;
  m_front.y += quantity.y;
  m_front.z += quantity.z;
}

void Camera::set_position(const Vector3& position)
{
  this->position = position;
  m_position.x = position.x;
  m_position.y = position.y * std::sqrt(2.0f) + camera_z;
  m_position.z = position.z + camera_z;

  m_front.x = position.x;
  m_front.y = position.y;
  m_front.z = position.z;
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

  m_projection = glm::ortho(m_frustrum_left, m_frustrum_right, m_frustrum_bottom, m_frustrum_top, m_near, m_far);

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
