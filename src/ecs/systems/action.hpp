#pragma once

#include <entt/entity/registry.hpp>
#include <map>

#include "core/input_manager.hpp"
#include "ui/types.hpp"
#include "world/society/job_type.hpp"
#include "world/target.hpp"

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
    SelectHarvestTarget = 0,
    SelectBreakTarget,
    SelectDigTarget,
    SelectHutTarget,
    SelectStorageTarget,
    Closed,
    Open,
  };

  World& m_world;
  ui::UIManager& m_ui_manager;

  static const ui::ItemList<uint32_t> m_menu_items;
  ui::ItemList<uint32_t> m_actions{};
  ui::ActionMenu* m_action_menu = nullptr;
  ui::Notification* m_notification = nullptr;
  EventEmitter& m_event_emitter;

  std::vector<entt::entity> m_selected_entities{};
  ActionMenuState m_state = ActionMenuState::Closed;
  InputManager& m_input_manager = InputManager::get_instance();

  void m_update_action_menu();
  void m_update_selecting_target(entt::registry& registry, const Camera& camera);
  void m_update_closed_menu(entt::registry& registry, const Camera& camera);
  void m_open_action_menu();
  void m_close_action_menu();
  void m_dispose();
  void m_select_tile_target(const Vector3i& tile_position, const JobType job_type, entt::registry& registry);
  void m_assign_job(const entt::entity job,
                    const Vector3i& position,
                    entt::registry& registry,
                    const entt::entity entity);
  bool m_has_qualities_required(const std::vector<std::string>& qualities_required,
                                const entt::entity entity,
                                entt::registry& registry);
  bool m_has_consumables(const std::map<uint32_t, uint32_t>& consumables, entt::registry& registry);
  std::function<void(const uint32_t)> m_on_select_generic_action
      = [this](const uint32_t i) { m_state = static_cast<ActionMenuState>(i); };

  void m_select_harvest_target(const Camera& camera, entt::registry& registry);
  void m_select_break_target(const Camera& camera, entt::registry& registry);
  void m_select_dig_target(const Camera& camera, entt::registry& registry);
};
}  // namespace dl
