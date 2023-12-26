#include "./camera.hpp"

#include <spdlog/spdlog.h>

#include <cmath>

#include "core/display.hpp"

namespace
{
constexpr float MAX_ZOOM = 4.0f;
constexpr float MIN_ZOOM = 0.25f;
}  // namespace

namespace dl
{
Camera::Camera(const Display& display)
{
  const auto& display_size = display.get_size();
  set_size({static_cast<double>(display_size.x), static_cast<double>(display_size.y)});
}

glm::mat4 Camera::get_view_matrix() const
{
  if (m_resize_view_matrix)
  {
    auto view_matrix = glm::lookAt(m_position, m_center, m_up);
    return glm::scale(view_matrix, glm::vec3(1.0f, m_scaling_factor, m_scaling_factor));
  }

  return glm::lookAt(m_position, m_center, m_up);
}

const Vector2i Camera::get_position_in_tiles() const
{
  assert(m_grid_size.x != 0.f && m_grid_size.y != 0.f && "Tile size was not set");

  return Vector2i{position.x / m_grid_size.x, position.y / m_grid_size.y};
}

void Camera::move(const Vector3& quantity)
{
  position.x += quantity.x * zoom;
  position.y += quantity.y * zoom;
  position.z += quantity.z * zoom;
  m_position.x += quantity.x;
  m_position.y += quantity.y * m_scaling_factor;
  m_position.z += quantity.z;

  m_calculate_center();
}

void Camera::set_position(const Vector3& position)
{
  this->position = position;
  m_position.x = position.x;
  m_position.y = position.y * m_scaling_factor + m_camera_z;
  m_position.z = position.z + m_camera_z;

  m_calculate_center();
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

  m_calculate_projection_matrix();

  m_size.x = std::abs(right - left);
  m_size.y = std::abs(top - bottom);

  if (m_grid_size.x > 0 && m_grid_size.y > 0)
  {
    m_size_in_tiles.x = std::ceil(m_size.x / m_grid_size.x);
    m_size_in_tiles.y = std::ceil(m_size.y / m_grid_size.y);
  }
}

void Camera::set_tile_size(const Vector2i& size)
{
  assert(size.x > 0 && "Tile width must be greather than 0");
  assert(size.y > 0 && "Tile height must be greather than 0");

  m_tile_size = size;
  m_grid_size = Vector2i{size.x * zoom, size.y * zoom};
  m_size_in_tiles.x = std::ceil(m_size.x / m_grid_size.x);
  m_size_in_tiles.y = std::ceil(m_size.y / m_grid_size.y);
}

void Camera::set_yaw(const float yaw)
{
  this->yaw = yaw;
  m_calculate_center();
}

void Camera::set_pitch(const float pitch)
{
  this->pitch = pitch;
  m_scaling_factor = 1.0 / std::sin(glm::radians(-pitch));
  m_calculate_center();
}

void Camera::set_zoom(const float zoom)
{
  if (zoom > MAX_ZOOM || zoom < MIN_ZOOM)
  {
    return;
  }

  this->zoom = zoom;
  m_calculate_projection_matrix();
}

void Camera::zoom_in()
{
  if (zoom >= MAX_ZOOM)
  {
    return;
  }

  position.x += m_size.x * zoom / 2.0;
  position.y += m_size.y * zoom / 2.0;
  m_position.x += (m_frustrum_right - m_frustrum_left) * 0.5f * 0.5f;
  m_position.y += 100.0f;
  /* m_position.y -= m_size.y*zoom/2.0*m_scaling_factor; */

  /* m_position.x += m_size.x*zoom; */
  /* m_position.y += m_size.y*zoom; */
  /* m_position.x += (static_cast<float>(m_size.x) * (1.0f / zoom - 1.0f))*zoom; */
  /* m_position.y += (static_cast<float>(m_size.y) * (1.0f / zoom - 1.0f))*zoom; */
  /* m_position.x += static_cast<float>(m_size.x) / 2.0f * (1.0f / zoom - 1.0f)*1.0f/zoom; */
  /* m_position.y += static_cast<float>(m_size.y) / 2.0f * (1.0f / zoom - 1.0f)*1.0f/zoom; */
  /* m_position.x += m_size.x*zoom; */

  zoom *= 2.0f;
  zoom = std::min(zoom, MAX_ZOOM);

  /* m_position.x += (m_frustrum_right - m_frustrum_left) * 0.5f * 0.5f; */
  /* m_position.y += (m_frustrum_bottom - m_frustrum_top) * 0.5f * 0.5f * m_scaling_factor; */
  /* m_position.z -= 10.f; */
  /* m_position.z += (m_frustrum_bottom - m_frustrum_top) * 0.5f * 0.5f * m_scaling_factor; */
  m_calculate_center();
  m_calculate_projection_matrix();
}

void Camera::zoom_out()
{
  if (zoom <= MIN_ZOOM)
  {
    return;
  }

  zoom *= 0.5f;
  zoom = std::max(zoom, MIN_ZOOM);

  /* position.x -= m_size.x*zoom/2.0; */
  /* position.y -= m_size.y*zoom/2.0; */
  /* m_position.y -= m_size.y*zoom/2.0*m_scaling_factor; */

  /* m_position.x -= m_size.x*zoom; */
  /* m_position.y -= m_size.y*zoom; */
  /* m_position.x -= (static_cast<float>(m_size.x) * (1.0f / zoom - 1.0f))*zoom; */
  /* m_position.y -= (static_cast<float>(m_size.y) * (1.0f / zoom - 1.0f))*zoom; */
  /* m_position.x -= static_cast<float>(m_size.x) / 2.0f * (1.0f / zoom - 1.0f)*1.0f/zoom; */
  /* m_position.y -= static_cast<float>(m_size.y) / 2.0f * (1.0f / zoom - 1.0f)*1.0f/zoom; */
  m_calculate_projection_matrix();
  /* m_position.x -= (m_frustrum_right - m_frustrum_left) * 0.5f * 0.5f; */
  m_position.y -= 100.0f * m_scaling_factor * (1.0f / zoom);

  /* m_position.x -= (m_frustrum_right - m_frustrum_left) * 0.5f * 0.5f; */
  /* m_position.y -= (m_frustrum_bottom - m_frustrum_top) * 0.5f * 0.5f * m_scaling_factor; */
  /* m_position.z += 100.f; */
  m_calculate_center();
}

void Camera::reset_zoom() { zoom = DEFAULT_ZOOM; }

void Camera::m_calculate_center()
{
  glm::vec3 direction{};
  direction.x = std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch));
  direction.y = std::sin(glm::radians(pitch));
  direction.z = std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch));

  m_center = m_position + glm::normalize(std::move(direction));
}

void Camera::m_calculate_projection_matrix()
{
  assert(zoom > 0.0f);

  m_grid_size.x = m_tile_size.x * zoom;
  m_grid_size.y = m_tile_size.y * zoom;
  m_size_in_tiles.x = std::ceil(m_size.x / m_grid_size.x);
  m_size_in_tiles.y = std::ceil(m_size.y / m_grid_size.y);

  /* m_frustrum_left = -static_cast<float>(m_size.x) / 2.0f * (1.0f / zoom - 1.0f); */
  m_frustrum_right = m_size.x + static_cast<float>(m_size.x) * (1.0f / zoom - 1.0f);
  m_frustrum_bottom = m_size.y + static_cast<float>(m_size.y) * (1.0f / zoom - 1.0f),
  /* m_frustrum_top = -static_cast<float>(m_size.y) / 2.0f * (1.0f / zoom - 1.0f), */

      /* m_size.x = m_frustrum_right - m_frustrum_left; */
      /* m_size.y = m_frustrum_bottom - m_frustrum_top; */

      projection_matrix =
          glm::ortho(m_frustrum_left, m_frustrum_right, m_frustrum_bottom, m_frustrum_top, m_near, m_far);

  /* position.x = m_position.x + m_frustrum_left*zoom; */
  /* position.y = m_position.y - m_camera_z + m_frustrum_top*zoom; */
  /* position.x = m_position.x + m_frustrum_left*zoom; */
  /* position.y = m_position.y*(1.0f/m_scaling_factor) - m_camera_z*(1.0f/m_scaling_factor) + m_frustrum_top*zoom; */
}

}  // namespace dl
