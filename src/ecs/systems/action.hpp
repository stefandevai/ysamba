#pragma once

#include <entt/entity/registry.hpp>

#include "core/input_manager.hpp"

namespace dl::ui
{
class UIManager;
class ActionMenu;
class Label;
}  // namespace dl::ui

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
    SelectHarvestTarget = 0,
    SelectPickupTarget,
    SelectBreakTarget,
    Closed,
    Open,
  };

  World& m_world;
  ui::UIManager& m_ui_manager;

  static const std::vector<std::string> menu_items;
  std::shared_ptr<ui::ActionMenu> m_action_menu = nullptr;
  int m_action_menu_id = -1;
  std::shared_ptr<ui::Label> m_select_target_label = nullptr;
  int m_select_target_label_id = -1;

  std::vector<entt::entity> m_selected_entities{};
  ActionMenuState m_state = ActionMenuState::Closed;
  std::shared_ptr<InputManager> m_input_manager = InputManager::get_instance();

  void m_update_action_menu();
  void m_update_selecting_target(entt::registry& registry, const Camera& camera);
  void m_update_closed_menu(entt::registry& registry, const Camera& camera);
  void m_show_select_target_text();
  void m_open_action_menu();
  void m_close_action_menu();
  void m_close_select_target();
  void m_dispose();
  void m_select_harvest_target(const int tile_x, const int tile_y, entt::registry& registry);
  void m_select_pickup_target(const int tile_x, const int tile_y, entt::registry& registry);
  void m_select_break_target(const int tile_x, const int tile_y, entt::registry& registry);
};
}  // namespace dl
