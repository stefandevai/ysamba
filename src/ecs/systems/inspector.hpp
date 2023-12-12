#pragma once

#include <entt/entity/registry.hpp>

#include "core/input_manager.hpp"
#include "core/maths/vector.hpp"

namespace dl::ui
{
class UIManager;
class Inspector;
}  // namespace dl::ui

namespace dl
{
class World;
struct TileData;
class Camera;

class InspectorSystem
{
 public:
  InspectorSystem(World& world, ui::UIManager& ui_manager);

  void update(entt::registry& registry, const Camera& camera);

 private:
  enum class State
  {
    Active,
    Inactive,
  };

  State m_state = State::Inactive;
  World& m_world;
  ui::UIManager& m_ui_manager;
  ui::Inspector& m_inspector;
  InputManager& m_input_manager = InputManager::get_instance();
  Vector2i m_last_mouse_position{0, 0};
  Vector3 m_last_camera_position{0., 0., 0.};
  entt::entity m_target_quad = entt::null;

  void m_update_input(entt::registry& registry);
  void m_update_inspector_content(const entt::entity entity, entt::registry& registry);
  void m_update_inspector_content(const TileData& tile_data);
  void m_destroy_inspector();
  inline bool m_is_valid() const;
};
}  // namespace dl
