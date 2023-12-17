#pragma once

#include <entt/entity/registry.hpp>

#include "core/input_manager.hpp"
#include "ui/types.hpp"
#include "world/society/job.hpp"

namespace dl::ui
{
class UIManager;
class ItemSelection;
class Label;
}  // namespace dl::ui

namespace dl
{
class World;
class Camera;
struct Vector2i;

class DropSystem
{
 public:
  DropSystem(World& world, ui::UIManager& ui_manager);
  void update(entt::registry& registry, const Camera& camera);

 private:
  enum class DropMenuState
  {
    SelectingTarget,
    Closed,
    Open,
  };

  World& m_world;
  ui::UIManager& m_ui_manager;

  static const ui::ItemList<uint32_t> m_menu_items;
  ui::ItemList<std::pair<entt::entity, entt::entity>> m_items{};
  ui::ItemSelection* m_drop_menu = nullptr;
  ui::Label* m_select_target_label = nullptr;
  entt::entity m_selected_entity = entt::null;
  entt::entity m_target_item = entt::null;

  DropMenuState m_state = DropMenuState::Closed;
  InputManager& m_input_manager = InputManager::get_instance();

  void m_update_drop_menu();
  void m_update_selecting_target(entt::registry& registry, const Camera& camera);
  void m_update_closed_menu(entt::registry& registry);
  void m_show_select_target_text();
  void m_open_drop_menu();
  void m_close_drop_menu();
  void m_close_select_target();
  void m_dispose();
  void m_create_job(const JobType job_type,
                    const uint32_t id,
                    const Vector2i& position,
                    entt::registry& registry,
                    entt::entity entity);
};
}  // namespace dl
