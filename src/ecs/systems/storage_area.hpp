#pragma once

#include <entt/entity/registry.hpp>

#include "core/input_manager.hpp"
#include "core/maths/vector.hpp"

namespace dl::ui
{
class UIManager;
class Notification;
}  // namespace dl::ui

namespace dl
{
class World;
struct EventEmitter;
class Camera;

class StorageAreaSystem
{
 public:
  StorageAreaSystem(World& world, EventEmitter& event_emitter, ui::UIManager& ui_manager);

  void update(entt::registry& registry);
  void update_state(entt::registry& registry, const Camera& camera);

 private:
  enum class State
  {
    None,
    SelectArea,
  };

  State m_state = State::None;
  World& m_world;
  InputManager& m_input_manager = InputManager::get_instance();
  ui::UIManager& m_ui_manager;
  ui::Notification* m_notification = nullptr;

  // Caches hut size in preview between updates so that we don't have to redraw the preview every frame
  Vector3i m_last_begin{};
  Vector3i m_last_end{};

  void m_update_select_area(entt::registry& registry, const Camera& camera);
  bool m_is_area_valid(const Vector3i& begin, const Vector3i& end);
  void m_select_area(entt::registry& registry, const Camera& camera);
  void m_preview_area(const Vector3i& begin, const Vector3i& end, entt::registry& registry);
  void m_set_storage_area(const Vector3i& begin, const Vector3i& end, entt::registry& registry);
  void m_dispose();
};
}  // namespace dl
