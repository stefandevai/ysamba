#pragma once

#include <entt/entity/registry.hpp>

#include "core/input_manager.hpp"
#include "ui/types.hpp"

namespace dl::ui
{
class UIManager;
class Inventory;
}  // namespace dl::ui

namespace dl
{
class World;
class Camera;
struct Vector2i;

class InventorySystem
{
 public:
  InventorySystem(World& world, ui::UIManager& ui_manager);
  void update(entt::registry& registry);

 private:
  enum class InventoryState
  {
    Closed,
    Open,
  };

  World& m_world;
  ui::UIManager& m_ui_manager;
  std::vector<entt::entity> m_carried_items{};
  std::vector<entt::entity> m_weared_items{};
  ui::ItemList<uint32_t> m_carried_items_names{};
  ui::ItemList<uint32_t> m_weared_items_names{};
  ui::Inventory* m_inventory = nullptr;
  InventoryState m_state = InventoryState::Closed;
  InputManager& m_input_manager = InputManager::get_instance();

  void m_update_inventory(entt::registry& registry);
  void m_update_closed_inventory(entt::registry& registry);
  void m_open_inventory(entt::registry& registry);
  void m_close_inventory();
  void m_dispose();
  void m_update_items(entt::registry& registry);
};
}  // namespace dl
