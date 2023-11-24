#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace dl
{
enum class CameraState
{
  IDLE,
  FOLLOW,
};

class Camera
{
 public:
  Camera();

  void move(const float x, const float y, const float z = 0.f);
  const glm::vec3& get_position() const { return m_position; }
  const glm::vec2 get_position_in_tiles() const;
  glm::mat4 get_view_matrix() const { return glm::lookAt(m_position, m_front, m_up); }
  glm::mat4 get_default_view_matrix() const { return glm::lookAt({0.f, 0.f, 500.f}, {0.f, 0.f, -1.f}, m_up); }
  glm::mat4 get_projection_matrix() const
  {
    return glm::ortho(m_frustrum_left, m_frustrum_right, m_frustrum_bottom, m_frustrum_top, m_near, m_far);
  }
  glm::mat4 get_vp_matrix() const
  {
    return (glm::ortho(m_frustrum_left, m_frustrum_right, m_frustrum_bottom, m_frustrum_top, m_near, m_far) *
            glm::lookAt(m_position, m_front, m_up));
  }
  const glm::vec3& get_saved_position() const { return m_saved_position; }
  const glm::vec2& get_size() const { return m_size; }
  const glm::vec2& get_size_in_tiles() const { return m_size_in_tiles; }
  const glm::vec2& get_tile_size() const { return m_tile_size; }
  void set_position(const float x, const float y, const float z);
  void set_size(const float width, const float height);
  void set_frustrum(const float left, const float right, const float bottom, const float top);
  void set_tile_size(const float width, const float height);
  void save_position() { m_saved_position = m_position; }

 private:
  float m_frustrum_left = 0.0f;
  float m_frustrum_right = 0.0f;
  float m_frustrum_bottom = 0.0f;
  float m_frustrum_top = 0.0f;
  float m_near = 0.1f;
  float m_far = 1000.0f;
  glm::vec3 m_position{0.0f, 0.0f, 500.0f};
  glm::vec3 m_front{0.0f, 0.0f, -1.0f};
  const glm::vec3 m_up{0.0f, 1.0f, 0.0f};
  glm::vec3 m_saved_position{0.0f};
  glm::vec2 m_size;
  glm::vec2 m_tile_size{0.f, 0.f};
  glm::vec2 m_size_in_tiles{0.f, 0.f};
  // CameraState m_state = CameraState::IDLE;
};
}  // namespace dl
