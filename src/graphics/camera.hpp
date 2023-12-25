#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "core/maths/vector.hpp"

namespace dl
{
class Display;

enum class CameraState
{
  IDLE,
  FOLLOW,
};

class Camera
{
 public:
  float yaw = -90.0f;
  float pitch = -45.0f;
  Vector3 position{0.0, 0.0, 0.0};

  Camera() = default;
  Camera(const Display& display);

  void move(const Vector3& quantity);
  const Vector3& get_position() const { return position; }
  const Vector2i get_position_in_tiles() const;
  glm::mat4 get_view_matrix() const;
  glm::mat4 get_projection_matrix() const { return m_projection; }
  const Vector3& get_saved_position() const { return m_saved_position; }
  const Vector2& get_size() const { return m_size; }
  const Vector2i& get_size_in_tiles() const { return m_size_in_tiles; }
  const Vector2i& get_tile_size() const { return m_tile_size; }
  void set_position(const Vector3& position);
  void set_size(const Vector2& size);
  void set_frustrum(const float left, const float right, const float bottom, const float top);
  void set_tile_size(const Vector2i& size);
  void set_yaw(const float yaw);
  void set_pitch(const float pitch);

 private:
  friend class CameraInspector;

  float m_frustrum_left = 0.0f;
  float m_frustrum_right = 0.0f;
  float m_frustrum_bottom = 0.0f;
  float m_frustrum_top = 0.0f;
  float m_near = 1.0f;
  float m_far = 2000.0f;
  double m_camera_z = 600.0;
  glm::vec3 m_position{0.0, m_camera_z, m_camera_z};
  glm::vec3 m_center =
      m_position + glm::vec3{glm::normalize(glm::vec3{std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch)),
                                                      std::sin(glm::radians(pitch)),
                                                      std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch))})};
  const glm::vec3 m_up{0.0f, 1.0f, 0.0f};
  double m_scaling_factor = 1.0 / std::sin(glm::radians(-pitch));
  glm::mat4 m_projection = glm::mat4{1.f};
  Vector3 m_saved_position{0., 0., 0.};
  Vector2 m_size{};
  Vector2i m_tile_size{0, 0};
  Vector2i m_size_in_tiles{0, 0};
  bool m_resize_view_matrix = true;

  void m_calculate_center();
};
}  // namespace dl
