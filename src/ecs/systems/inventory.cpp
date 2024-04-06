#include "./inventory.hpp"

#include <spdlog/spdlog.h>

#include <entt/core/hashed_string.hpp>
#include <iterator>

#include "ecs/components/carried_items.hpp"
#include "ecs/components/item.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/selectable.hpp"
#include "ecs/components/society_agent.hpp"
#include "ecs/components/storage_area.hpp"
#include "ecs/components/weared_items.hpp"
#include "ecs/components/wielded_items.hpp"
#include "ui/compositions/inventory.hpp"
#include "ui/compositions/society_inventory.hpp"
#include "ui/ui_manager.hpp"
#include "world/world.hpp"

namespace dl
{
InventorySystem::InventorySystem(World& world, ui::UIManager& ui_manager) : m_world(world), m_ui_manager(ui_manager)
{
  auto on_select = [](const int i)
  {
    // TODO: Open item details menu
    (void)i;
  };

  m_selected_inventory = m_ui_manager.emplace<ui::Inventory>(on_select);
  m_society_inventory = m_ui_manager.emplace<ui::SocietyInventory>(on_select);
}

void InventorySystem::update(entt::registry& registry)
{
  if (m_state == State::OpenSelected)
  {
    m_update_selected_inventory();
  }
  else if (m_state == State::OpenSociety)
  {
    m_update_society_inventory();
  }
  else if (m_state == State::Closed)
  {
    m_update_closed_inventory(registry);
  }
}

// Updates inventory for selected entities
void InventorySystem::m_update_selected_inventory()
{
  using namespace entt::literals;

  if (!m_input_manager.is_context("inventory"_hs))
  {
    return;
  }

  if (m_input_manager.poll_action("close_inventory"_hs))
  {
    m_dispose();
  }
}

// Updates inventory for all society agents and storage area
void InventorySystem::m_update_society_inventory()
{
  using namespace entt::literals;

  if (!m_input_manager.is_context("inventory"_hs))
  {
    return;
  }

  if (m_input_manager.poll_action("close_inventory"_hs))
  {
    m_dispose();
  }
}

// Updates the closed inventory state
void InventorySystem::m_update_closed_inventory(entt::registry& registry)
{
  using namespace entt::literals;

  const auto& current_context = m_input_manager.get_current_context();

  if (current_context == nullptr || current_context->key != "gameplay"_hs)
  {
    return;
  }

  if (m_input_manager.poll_action("open_inventory"_hs))
  {
    const auto entities = m_get_selected_entities(registry);

    if (entities.empty())
    {
      m_open_society_inventory(registry);
      m_state = State::OpenSociety;
    }
    else
    {
      m_open_selected_inventory(registry, entities);
      m_state = State::OpenSelected;
    }

    m_input_manager.push_context("inventory"_hs);
  }
}

// Opens inventory for selected entities
void InventorySystem::m_open_selected_inventory(entt::registry& registry,
                                                const std::vector<entt::entity>& selected_entities)
{
  assert(m_selected_inventory != nullptr);

  if (m_selected_inventory->state != ui::UIComponent::State::Hidden)
  {
    return;
  }

  const auto weared_items = m_get_weared_items(registry, selected_entities);
  const auto carried_items = m_get_carried_items(registry, selected_entities);

  m_weared_items_names.clear();
  m_weared_items_names.reserve(weared_items.size());
  m_carried_items_names.clear();
  m_carried_items_names.reserve(carried_items.size());

  for (const auto entity : weared_items)
  {
    if (!registry.all_of<Item>(entity))
    {
      continue;
    }

    const auto& item = registry.get<Item>(entity);
    const auto& item_data = m_world.get_item_data(item.id);
    m_weared_items_names.push_back({static_cast<uint32_t>(entity), item_data.name});
  }

  for (const auto entity : carried_items)
  {
    if (!registry.all_of<Item>(entity))
    {
      continue;
    }

    const auto& item = registry.get<Item>(entity);
    const auto& item_data = m_world.get_item_data(item.id);
    m_carried_items_names.push_back({static_cast<uint32_t>(entity), item_data.name});
  }

  m_selected_inventory->set_weared_items(m_weared_items_names);
  m_selected_inventory->set_carried_items(m_carried_items_names);
  m_selected_inventory->show();
}

// Opens inventory for all society agents and storage area
void InventorySystem::m_open_society_inventory(entt::registry& registry)
{
  assert(m_society_inventory != nullptr);

  if (m_society_inventory->state != ui::UIComponent::State::Hidden)
  {
    return;
  }

  const auto society_items = m_get_society_items(registry);
  m_society_items_names.clear();

  for (const auto entity : society_items)
  {
    if (!registry.all_of<Item>(entity))
    {
      continue;
    }

    const auto& item = registry.get<Item>(entity);
    const auto& item_data = m_world.get_item_data(item.id);
    m_society_items_names.push_back({static_cast<uint32_t>(entity), item_data.name});
  }

  m_society_inventory->set_items(m_society_items_names);
  m_society_inventory->show();
}

// Closes any inventory open
void InventorySystem::m_close_inventory()
{
  switch (m_state)
  {
  case State::OpenSelected:
  {
    m_selected_inventory->hide();
    break;
  }
  case State::OpenSociety:
  {
    m_society_inventory->hide();
    break;
  }
  default:
  {
    break;
  }
  }
}

// Removes UI elements and pops input context
void InventorySystem::m_dispose()
{
  m_close_inventory();
  m_state = State::Closed;
  m_input_manager.pop_context();
}

std::vector<entt::entity> InventorySystem::m_get_selected_entities(entt::registry& registry)
{
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

  return selected_entities;
}

// Gets weared items by society agents. If selected_entities is empty, all society agents are considered
std::vector<entt::entity> InventorySystem::m_get_weared_items(entt::registry& registry,
                                                              const std::vector<entt::entity>& selected_entities)
{
  std::vector<entt::entity> items{};

  const auto push_items = [&registry, &items](const entt::entity entity)
  {
    if (registry.all_of<WieldedItems>(entity))
    {
      const auto& wielded_items = registry.get<WieldedItems>(entity);
      const auto left_hand = wielded_items.left_hand;
      const auto right_hand = wielded_items.right_hand;

      if (registry.valid(left_hand))
      {
        items.push_back(left_hand);
      }
      if (registry.valid(right_hand) && right_hand != left_hand)
      {
        items.push_back(right_hand);
      }
    }
    if (registry.all_of<WearedItems>(entity))
    {
      const auto& weared_items = registry.get<WearedItems>(entity);
      items.insert(items.end(), weared_items.items.begin(), weared_items.items.end());
    }
  };

  // Get weared items for every society agent
  if (selected_entities.empty())
  {
    auto selectable_view = registry.view<Selectable>();

    for (const auto entity : selectable_view)
    {
      push_items(entity);
    }
  }
  // Get weared items only for selected society agents
  else
  {
    for (const auto entity : selected_entities)
    {
      push_items(entity);
    }
  }

  return items;
}

// Gets carried items by society agents. If selected_entities is empty, all society agents are considered
std::vector<entt::entity> InventorySystem::m_get_carried_items(entt::registry& registry,
                                                               const std::vector<entt::entity>& selected_entities)
{
  std::vector<entt::entity> items{};

  const auto push_items = [&registry, &items](const entt::entity entity)
  {
    if (registry.all_of<CarriedItems>(entity))
    {
      const auto& carried_items = registry.get<CarriedItems>(entity);
      items.insert(items.begin(), carried_items.items.begin(), carried_items.items.end());
    }
  };

  // Get carried items for every society agent
  if (selected_entities.empty())
  {
    auto selectable_view = registry.view<Selectable>();

    for (const auto entity : selectable_view)
    {
      push_items(entity);
    }
  }
  // Get carried items only for selected society agents
  else
  {
    for (const auto entity : selected_entities)
    {
      push_items(entity);
    }
  }

  return items;
}

std::vector<entt::entity> InventorySystem::m_get_storage_items(entt::registry& registry)
{
  std::vector<entt::entity> items{};

  auto storage_view = registry.view<StorageArea>();

  for (const auto entity : storage_view)
  {
    const auto& position = registry.get<Position>(entity);
    const auto entities = m_world.spatial_hash.get_all_by_component<Item>(position.x, position.y, position.z, registry);
    items.insert(items.end(), std::make_move_iterator(entities.begin()), std::make_move_iterator(entities.end()));
  }

  return items;
}

// Gets items in storage area and items carried by all society agents
std::vector<entt::entity> InventorySystem::m_get_society_items(entt::registry& registry)
{
  std::vector<entt::entity> items{};

  const auto storage_items = m_get_storage_items(registry);

  items.insert(
      items.end(), std::make_move_iterator(storage_items.begin()), std::make_move_iterator(storage_items.end()));

  const auto carried_items = m_get_carried_items(registry);

  items.insert(
      items.end(), std::make_move_iterator(carried_items.begin()), std::make_move_iterator(carried_items.end()));

  return items;
}

}  // namespace dl
