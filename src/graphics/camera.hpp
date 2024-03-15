#pragma once

#include <entt/entity/entity.hpp>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "core/maths/vector.hpp"
#include "ecs/components/position.hpp"

namespace
{
constexpr float DEFAULT_ZOOM = 1.0f;
}

namespace dl
{
class Display;
struct EventEmitter;

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
  float zoom = DEFAULT_ZOOM;
  Vector3 movement_offset{0.0, 0.0, 0.0};
  Vector3 view_position{0.0, 0.0, 0.0};
  Vector3i position_in_tiles{0, 0, 0};
  Vector3i center_in_tiles{0, 0, 0};
  glm::mat4 projection_matrix = glm::mat4{1.f};
  glm::mat4 view_matrix = glm::mat4{1.f};
  glm::mat4 view_projection_matrix = glm::mat4{1.f};
  entt::entity target = entt::null;
  bool dirty = true;

  Camera() = default;
  Camera(const Display& display);

  void update(const float dt);
  // TODO: Implement different cameras so that other scenes can have a simpler update
  // then merge this method to the main update method
  void update_target(const entt::registry& registry);
  void update_dirty();
  void move(const Vector3& quantity);
  void move_in_grid(const Vector3i& quantity);
  const Vector3& get_position() const;
  const Vector3i& get_position_in_tiles() const;
  const glm::mat4& get_view_matrix() const;
  const Vector3& get_saved_position() const { return m_saved_position; }
  const Vector2& get_size() const { return m_size; }
  const Vector2i& get_size_in_tiles() const { return m_size_in_tiles; }
  const Vector2i& get_grid_size() const { return m_grid_size; }
  void set_position(const Vector2& position);
  void set_size(const Vector2& size);
  void set_frustum(const float left, const float right, const float bottom, const float top);
  void set_tile_size(const Vector2i& size);
  void set_yaw(const float yaw);
  void set_pitch(const float pitch);
  void set_zoom(const float zoom);
  void set_target(const entt::entity target);
  void set_event_emitter(EventEmitter* emitter);
  void zoom_in();
  void zoom_out();
  void reset_zoom();

 private:
  friend class CameraInspector;

  EventEmitter* m_event_emitter = nullptr;

  float m_frustum_left = 0.0f;
  float m_frustum_right = 0.0f;
  float m_frustum_bottom = 0.0f;
  float m_frustum_top = 0.0f;
  float m_near = 1.0f;
  float m_far = 3000.0f;
  double m_camera_z = 2000.0;
  glm::vec3 m_position{0.0, m_camera_z, m_camera_z};
  glm::vec3 m_center
      = m_position
        + glm::vec3{glm::normalize(glm::vec3{std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch)),
                                             std::sin(glm::radians(pitch)),
                                             std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch))})};
  const glm::vec3 m_up{0.0f, 1.0f, 0.0f};
  double m_scaling_factor = 1.0 / std::sin(glm::radians(-pitch));
  Vector3 m_saved_position{0., 0., 0.};
  Vector2 m_size{};
  Vector2i m_tile_size{0, 0};
  Vector2i m_grid_size{0, 0};
  Vector2i m_size_in_tiles{0, 0};
  bool m_resize_view_matrix = true;
  Position m_last_target_position{};

  void m_calculate_center_position();
  void m_calculate_center_vector();
  void m_calculate_view_matrix();
  void m_calculate_position();
  void m_calculate_grid_size();
};
}  // namespace dl
