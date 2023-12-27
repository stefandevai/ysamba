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

void Camera::update(const float dt)
{
  (void)dt;

  if (dirty)
  {
    m_calculate_center();
    m_calculate_view_matrix();
    m_calculate_position();
    m_calculate_grid_size();
    dirty = false;
  }
}

const glm::mat4& Camera::get_view_matrix() const { return view_matrix; }

const Vector3& Camera::get_position() const { return view_position; }

const Vector2i& Camera::get_position_in_tiles() const { return position_in_tiles; }

void Camera::move(const Vector3& quantity)
{
  movement_offset.x += quantity.x * (1.0 / zoom);
  movement_offset.y += quantity.y * (1.0 / zoom);
  movement_offset.z += quantity.z * (1.0 / zoom);
  m_position.x += quantity.x * (1.0 / zoom);
  m_position.y += quantity.y * m_scaling_factor * (1.0 / zoom);
  m_position.z += quantity.z * (1.0 / zoom);

  dirty = true;
}

void Camera::move_in_grid(const Vector3i& quantity)
{
  move(Vector3{quantity.x * m_grid_size.x, quantity.y * m_grid_size.y, quantity.z * m_grid_size.y});
}

void Camera::set_position(const Vector3& position)
{
  (void)position;

  // TODO
}

void Camera::set_size(const Vector2& size)
{
  m_size = size;
  set_frustrum(0.f, size.x, size.y, 0.f);
  dirty = true;
}

void Camera::set_frustrum(const float left, const float right, const float bottom, const float top)
{
  m_frustrum_left = left;
  m_frustrum_right = right;
  m_frustrum_bottom = bottom;
  m_frustrum_top = top;

  m_size.x = std::abs(right - left);
  m_size.y = std::abs(top - bottom);

  if (m_grid_size.x > 0 && m_grid_size.y > 0)
  {
    m_size_in_tiles.x = std::ceil(m_size.x / m_grid_size.x);
    m_size_in_tiles.y = std::ceil(m_size.y / m_grid_size.y);
  }

  projection_matrix = glm::ortho(m_frustrum_left, m_frustrum_right, m_frustrum_bottom, m_frustrum_top, m_near, m_far);
}

void Camera::set_tile_size(const Vector2i& size)
{
  assert(size.x > 0 && "Tile width must be greather than 0");
  assert(size.y > 0 && "Tile height must be greather than 0");

  m_tile_size = size;
  dirty = true;
}

void Camera::set_yaw(const float yaw)
{
  this->yaw = yaw;
  dirty = true;
}

void Camera::set_pitch(const float pitch)
{
  this->pitch = pitch;
  m_scaling_factor = 1.0 / std::sin(glm::radians(-pitch));
  dirty = true;
}

void Camera::set_zoom(const float zoom)
{
  if (zoom > MAX_ZOOM || zoom < MIN_ZOOM)
  {
    return;
  }

  this->zoom = zoom;

  dirty = true;
}

void Camera::zoom_in()
{
  if (zoom >= MAX_ZOOM)
  {
    return;
  }

  zoom *= 2.0f;
  zoom = std::min(zoom, MAX_ZOOM);

  dirty = true;
}

void Camera::zoom_out()
{
  if (zoom <= MIN_ZOOM)
  {
    return;
  }

  zoom *= 0.5f;
  zoom = std::max(zoom, MIN_ZOOM);

  dirty = true;
}

void Camera::reset_zoom()
{
  zoom = DEFAULT_ZOOM;
  dirty = true;
}

void Camera::m_calculate_center()
{
  glm::vec3 direction{};
  direction.x = std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch));
  direction.y = std::sin(glm::radians(pitch));
  direction.z = std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch));

  m_center = m_position + glm::normalize(std::move(direction));
}

void Camera::m_calculate_view_matrix()
{
  if (m_resize_view_matrix)
  {
    view_matrix = glm::lookAt(m_position, m_center, m_up);
    view_matrix = glm::scale(view_matrix, glm::vec3(1.0f, m_scaling_factor, m_scaling_factor));
    view_matrix = glm::translate(
        view_matrix, glm::vec3(movement_offset.x + m_size.x / 2.0f, movement_offset.y + m_size.y / 2.0f, 0.0f));
    view_matrix = glm::scale(view_matrix, glm::vec3(zoom, zoom, zoom));
    view_matrix = glm::translate(
        view_matrix, glm::vec3(-movement_offset.x - m_size.x / 2.0f, -movement_offset.y - m_size.y / 2.0f, 0.0f));
    return;
  }

  view_matrix = glm::lookAt(m_position, m_center, m_up);
}

void Camera::m_calculate_position()
{
  glm::vec3 inverse_position = view_matrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
  view_position.x = -inverse_position.x;
  view_position.y = -inverse_position.y;
  view_position.z = -inverse_position.z;
}

void Camera::m_calculate_grid_size()
{
  m_grid_size.x = m_tile_size.x * zoom;
  m_grid_size.y = m_tile_size.y * zoom;

  if (m_grid_size.x > 0.f && m_grid_size.y > 0.f)
  {
    m_size_in_tiles.x = std::ceil(m_size.x / m_grid_size.x);
    m_size_in_tiles.y = std::ceil(m_size.y / m_grid_size.y);
    position_in_tiles.x = view_position.x / m_grid_size.x;
    position_in_tiles.y = view_position.y / m_grid_size.y;
  }
}

}  // namespace dl
