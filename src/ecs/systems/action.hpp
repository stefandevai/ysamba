#pragma once

#include <entt/entity/registry.hpp>
#include <functional>
#include <map>

#include "core/input_manager.hpp"
#include "ui/types.hpp"
#include "world/society/job_type.hpp"
#include "world/target.hpp"

namespace dl::ai
{
class ActionManager;
}

namespace dl::ui
{
class UIManager;
class ActionMenu;
class Notification;
}  // namespace dl::ui

namespace dl
{
class World;
class Camera;
struct Vector2i;
struct EventEmitter;

class ActionSystem
{
 public:
  ActionSystem(World& world, ui::UIManager& ui_manager, EventEmitter& event_emitter);
  void update(entt::registry& registry, const Camera& camera);

 private:
  enum class ActionMenuState
  {
    SelectTarget,
    Closed,
    Open,
  };

  World& m_world;
  ui::UIManager& m_ui_manager;

  static const ui::ItemList<JobType> m_menu_items;
  ui::ItemList<JobType> m_actions{};
  ui::ActionMenu* m_action_menu = nullptr;
  ui::Notification* m_notification = nullptr;
  EventEmitter& m_event_emitter;

  std::vector<entt::entity> m_selected_entities{};
  ActionMenuState m_state = ActionMenuState::Closed;
  JobType m_selected_job_type = JobType::None;
  InputManager& m_input_manager = InputManager::get_instance();

  // Caches selected area preview between updates so that we don't have to update it every frame
  Vector3i m_last_begin{};
  Vector3i m_last_end{};

  void m_update_action_menu();
  void m_update_selecting_target(entt::registry& registry, const Camera& camera);
  void m_update_closed_menu(entt::registry& registry, const Camera& camera);
  void m_open_action_menu();
  void m_close_action_menu();
  void m_dispose();
  std::function<void(const JobType)> m_on_select_generic_action = [this](const JobType job_type)
  {
    m_selected_job_type = job_type;
    m_state = ActionMenuState::SelectTarget;
  };

  void m_select_harvest_target(const Camera& camera, entt::registry& registry);
  void m_select_break_target(const Camera& camera, entt::registry& registry);
  void m_select_dig_target(const Camera& camera, entt::registry& registry);
  void m_select_area(entt::registry& registry,
                     const Camera& camera,
                     std::function<void(entt::registry&, const Vector3i&, const Vector3i&)> on_select);
  void m_preview_area(entt::registry& registry, const Vector3i& begin, const Vector3i& end);
  std::vector<entt::entity> m_select_available_entities(entt::registry& registry);
};
}  // namespace dl
