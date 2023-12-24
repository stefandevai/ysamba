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

  Camera();
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

 private:
  float m_frustrum_left = 0.0f;
  float m_frustrum_right = 0.0f;
  float m_frustrum_bottom = 0.0f;
  float m_frustrum_top = 0.0f;
  float m_near = 1.0f;
  float m_far = 2000.0f;
  double camera_z = 600.0;
  Vector3 m_position{0.0, camera_z, camera_z};
  glm::vec3 m_front{0.0f, 0.0f, -1.0f};
  const glm::vec3 m_up{0.0f, 1.0f, 0.0f};
  glm::mat4 m_projection = glm::mat4{1.f};
  Vector3 m_saved_position{0., 0., 0.};
  Vector2 m_size{};
  Vector2i m_tile_size{0, 0};
  Vector2i m_size_in_tiles{0, 0};
  bool m_resize_view_matrix = true;

  friend class CameraEditor;
};
}  // namespace dl
