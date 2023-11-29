#pragma once

#include <entt/entity/registry.hpp>

#include "core/input_manager.hpp"

namespace dl::ui
{
class UIManager;
}

namespace dl
{
class World;
class Camera;

class ActionSystem
{
 public:
  ActionSystem(World& world, ui::UIManager& ui_manager);
  void update(entt::registry& registry, const Camera& camera);

 private:
  enum class ActionMenuState
  {
    Closed,
    Open,
    SelectingTarget,
  };

  World& m_world;
  ui::UIManager& m_ui_manager;
  entt::entity m_menu_quad = entt::null;
  entt::entity m_menu_text = entt::null;
  entt::entity m_select_target_text = entt::null;
  std::vector<entt::entity> m_selected_entities{};
  ActionMenuState m_state = ActionMenuState::Closed;
  std::shared_ptr<InputManager> m_input_manager = InputManager::get_instance();

  void m_update_action_menu(entt::registry& registry, const Camera& camera);
  void m_update_selecting_target(entt::registry& registry, const Camera& camera);
  void m_update_closed_menu(entt::registry& registry, const Camera& camera);
  void m_show_select_target_text(entt::registry& registry, const Camera& camera);
  void m_open_action_menu(entt::registry& registry, const Camera& camera);
  void m_close_action_menu(entt::registry& registry);
};
}  // namespace dl
