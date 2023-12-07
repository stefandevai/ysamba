#include "./inventory.hpp"

#include <spdlog/spdlog.h>

#include "ecs/components/carried_items.hpp"
#include "ecs/components/item.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/selectable.hpp"
#include "ui/components/inventory.hpp"
#include "ui/ui_manager.hpp"
#include "world/world.hpp"

namespace dl
{
InventorySystem::InventorySystem(World& world, ui::UIManager& ui_manager) : m_world(world), m_ui_manager(ui_manager)
{
  auto on_select = [this](const int i) { m_state = static_cast<InventoryState>(i); };
  m_inventory = std::make_shared<ui::Inventory>(on_select);
}

void InventorySystem::update(entt::registry& registry)
{
  if (m_state == InventoryState::Open)
  {
    m_update_inventory(registry);
  }
  else if (m_state == InventoryState::Closed)
  {
    m_update_closed_inventory(registry);
  }
}

void InventorySystem::m_update_inventory(entt::registry& registry)
{
  if (m_inventory_id < 0)
  {
    m_open_inventory(registry);
  }

  if (m_input_manager->poll_action("close_inventory"))
  {
    m_dispose();
  }
}

void InventorySystem::m_update_closed_inventory(entt::registry& registry)
{
  const auto& current_context = m_input_manager->get_current_context();

  if (current_context == nullptr || current_context->key != "gameplay")
  {
    return;
  }

  if (m_input_manager->poll_action("open_inventory"))
  {
    m_update_items(registry);

    m_state = InventoryState::Open;
    m_input_manager->push_context("inventory");
  }
}

void InventorySystem::m_open_inventory(entt::registry& registry)
{
  assert(m_inventory != nullptr && m_inventory_id < 0);

  m_item_names.clear();
  m_item_names.reserve(m_items.size());

  for (const auto entity : m_items)
  {
    if (!registry.all_of<Item>(entity))
    {
      continue;
    }

    const auto& item = registry.get<Item>(entity);
    const auto& item_data = m_world.get_item_data(item.id);
    m_item_names.push_back(item_data.name);
  }

  m_inventory->set_items(m_item_names);

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

void InventorySystem::m_update_items(entt::registry& registry)
{
  m_items.clear();
  std::vector<entt::entity> selected_entities{};
  auto view = registry.view<Selectable>();

  for (const auto entity : view)
  {
    const auto& selectable = registry.get<Selectable>(entity);

    if (!selectable.selected)
    {
      continue;
    }

    selected_entities.push_back(entity);
  }

  if (!selected_entities.empty())
  {
    for (const auto entity : selected_entities)
    {
      if (!registry.all_of<CarriedItems>(entity))
      {
        continue;
      }

      const auto& items = registry.get<CarriedItems>(entity);

      m_items.insert(m_items.begin(), items.items.begin(), items.items.end());
    }
    return;
  }

  // TODO: Implement society inventory
}
}  // namespace dl
