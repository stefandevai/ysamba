#include "./inventory.hpp"

#include <spdlog/spdlog.h>

#include "ecs/components/item.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/selectable.hpp"
#include "ecs/components/weared_items.hpp"
#include "ui/components/action_menu.hpp"
#include "ui/ui_manager.hpp"
#include "world/world.hpp"

namespace dl
{
InventorySystem::InventorySystem(World& world, ui::UIManager& ui_manager) : m_world(world), m_ui_manager(ui_manager)
{
  auto on_select = [this](const int i) { m_state = static_cast<InventoryState>(i); };
  m_inventory = std::make_shared<ui::ActionMenu>(std::vector<std::string>{}, on_select);
}

void InventorySystem::update(entt::registry& registry, const Camera& camera)
{
  if (m_state == InventoryState::Open)
  {
    m_update_inventory();
  }
  else if (m_state == InventoryState::Closed)
  {
    m_update_closed_inventory();
  }
}

void InventorySystem::m_update_inventory()
{
  if (m_inventory_id < 0)
  {
    m_open_inventory();
  }

  if (m_input_manager->poll_action("close_inventory"))
  {
    m_dispose();
  }
}

void InventorySystem::m_update_closed_inventory()
{
  const auto& current_context = m_input_manager->get_current_context();

  if (current_context == nullptr || current_context->key != "gameplay")
  {
    return;
  }

  if (m_input_manager->poll_action("open_inventory"))
  {
    m_state = InventoryState::Open;
    m_input_manager->push_context("inventory");
  }
}

void InventorySystem::m_open_inventory()
{
  assert(m_inventory != nullptr && m_inventory_id < 0);
  m_inventory_id = m_ui_manager.add_component(m_inventory);
}

void InventorySystem::m_close_inventory()
{
  if (m_inventory_id >= 0)
  {
    m_ui_manager.remove_component(m_inventory_id);
    m_inventory_id = -1;
  }
}

void InventorySystem::m_dispose()
{
  m_close_inventory();
  m_state = InventoryState::Closed;
  m_input_manager->pop_context();
}

}  // namespace dl
