#pragma once

#include <entt/entity/registry.hpp>

#include "core/input_manager.hpp"

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
  void update(entt::registry& registry, const Camera& camera);

 private:
  enum class InventoryState
  {
    Closed,
    Open,
  };

  World& m_world;
  ui::UIManager& m_ui_manager;
  std::vector<entt::entity> m_items{};
  std::vector<std::string> m_item_names{};

  std::shared_ptr<ui::Inventory> m_inventory = nullptr;
  int m_inventory_id = -1;

  InventoryState m_state = InventoryState::Closed;
  std::shared_ptr<InputManager> m_input_manager = InputManager::get_instance();

  void m_update_inventory(entt::registry& registry);
  void m_update_closed_inventory(entt::registry& registry);
  void m_open_inventory(entt::registry& registry);
  void m_close_inventory();
  void m_dispose();
  void m_update_items(entt::registry& registry);
};
}  // namespace dl
