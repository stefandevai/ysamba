#pragma once

#include <entt/entity/registry.hpp>

#include "core/input_manager.hpp"
#include "ui/types.hpp"

namespace dl::ui
{
class UIManager;
class Inventory;
class SocietyInventory;
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
  enum class State
  {
    Closed,
    OpenSelected,
    OpenSociety,
  };

  World& m_world;
  ui::UIManager& m_ui_manager;
  ui::ItemList<uint32_t> m_carried_items_names{};
  ui::ItemList<uint32_t> m_weared_items_names{};
  ui::ItemList<uint32_t> m_society_items_names{};
  ui::Inventory* m_selected_inventory = nullptr;
  ui::SocietyInventory* m_society_inventory = nullptr;
  State m_state = State::Closed;
  InputManager& m_input_manager = InputManager::get_instance();

  void m_update_selected_inventory();
  void m_update_society_inventory();
  void m_update_closed_inventory(entt::registry& registry);
  void m_open_inventory(entt::registry& registry, const std::vector<entt::entity>& selected_entities);
  void m_open_society_inventory(entt::registry& registry);
  void m_close_inventory();
  void m_dispose();
  std::vector<entt::entity> m_get_selected_entities(entt::registry& registry);
  std::vector<entt::entity> m_get_weared_items(entt::registry& registry,
                                               const std::vector<entt::entity>& selected_entities = {});
  std::vector<entt::entity> m_get_carried_items(entt::registry& registry,
                                                const std::vector<entt::entity>& selected_entities = {});
  std::vector<entt::entity> m_get_storage_items(entt::registry& registry);
  std::vector<entt::entity> m_get_society_items(entt::registry& registry);
};
}  // namespace dl
