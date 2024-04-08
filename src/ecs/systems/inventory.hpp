#pragma once

#include <entt/entity/registry.hpp>

#include "core/input_manager.hpp"
#include "ui/types.hpp"

namespace dl::ui
{
class UIManager;
class GameplayModals;
}  // namespace dl::ui

namespace dl
{
class World;
class Camera;
struct Vector2i;

class InventorySystem
{
 public:
  InventorySystem(World& world, ui::GameplayModals& gameplay_modals);

  void update(entt::registry& registry);

 private:
  // Inventory state
  // Closed: No inventory is open
  // OpenSelected: Inventory is open for selected entities
  // OpenSociety: Inventory is open for all society agents and storage area
  enum class State
  {
    Closed,
    OpenSelected,
    OpenSociety,
  };

  World& m_world;

  ui::GameplayModals& m_gameplay_modals;

  // Caches names of items carried by society agents
  ui::ItemList<entt::entity> m_carried_items_names{};

  // Caches names of items weared and wielded by society agents
  ui::ItemList<entt::entity> m_weared_items_names{};

  // Caches storage items and carried items by all society agents
  ui::ItemList<entt::entity> m_society_items_names{};

  // Main inventory state
  State m_state = State::Closed;

  InputManager& m_input_manager = InputManager::get_instance();

  // Updates inventory for selected entities
  void m_update_selected_inventory();

  // Updates inventory for all society agents and storage area
  void m_update_society_inventory();

  // Updates the closed inventory state
  void m_update_closed_inventory(entt::registry& registry);

  // Opens inventory for selected entities
  void m_open_selected_inventory(entt::registry& registry, const std::vector<entt::entity>& selected_entities);

  // Opens inventory for all society agents and storage area
  void m_open_society_inventory(entt::registry& registry);

  std::vector<entt::entity> m_get_selected_entities(entt::registry& registry);

  // Gets weared items by society agents. If selected_entities is empty, all society agents are considered
  std::vector<entt::entity> m_get_weared_items(entt::registry& registry,
                                               const std::vector<entt::entity>& selected_entities = {});

  // Gets carried items by society agents. If selected_entities is empty, all society agents are considered
  std::vector<entt::entity> m_get_carried_items(entt::registry& registry,
                                                const std::vector<entt::entity>& selected_entities = {});
  std::vector<entt::entity> m_get_storage_items(entt::registry& registry);

  // Gets items in storage area and items carried by all society agents
  std::vector<entt::entity> m_get_society_items(entt::registry& registry);
};
}  // namespace dl
