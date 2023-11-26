#pragma once

#include <entt/entity/registry.hpp>

#include "core/input_manager.hpp"
#include "core/maths/vector.hpp"

namespace dl
{
class World;
class Camera;

class InspectorSystem
{
 public:
  InspectorSystem(World& world);

  void update(entt::registry& registry, const Camera& camera);

 private:
  World& m_world;
  entt::entity m_inspector_quad = entt::null;
  entt::entity m_inspector_text = entt::null;
  std::shared_ptr<InputManager> m_input_manager = InputManager::get_instance();
  Vector2i m_last_mouse_position{0, 0};
  Vector3 m_last_camera_position{0., 0., 0.};

  void m_update_inspector_content(const entt::entity entity, entt::registry& registry, const Camera& camera);
  void m_destroy_inspector(entt::registry& registry);
  inline bool m_is_valid(const entt::registry& registry) const;
};
}  // namespace dl
