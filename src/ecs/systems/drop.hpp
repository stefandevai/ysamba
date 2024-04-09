#pragma once

#include <entt/entity/registry.hpp>

#include "core/input_manager.hpp"
#include "ui/types.hpp"

namespace dl::ui
{
class GameplayModals;
class UIManager;
class Notification;
}  // namespace dl::ui

namespace dl
{
class World;
class Camera;
struct Vector2i;

class DropSystem
{
 public:
  DropSystem(World& world, ui::UIManager& ui_manager, ui::GameplayModals& gameplay_modals);
  void update(entt::registry& registry, const Camera& camera);

 private:
  enum class UIState
  {
    None,
    SelectingTarget,
  };

  World& m_world;
  ui::GameplayModals& m_gameplay_modals;
  ui::UIManager& m_ui_manager;

  static const ui::ItemList<uint32_t> m_menu_items;
  ui::ItemList<std::pair<entt::entity, entt::entity>> m_items{};
  ui::Notification* m_notification = nullptr;
  entt::entity m_selected_entity = entt::null;
  entt::entity m_target_item = entt::null;

  UIState m_ui_state = UIState::None;
  InputManager& m_input_manager = InputManager::get_instance();

  void m_update_selecting_target(entt::registry& registry, const Camera& camera);
  void m_process_input(entt::registry& registry);
  void m_dispose();
};
}  // namespace dl
