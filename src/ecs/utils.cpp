#include <entt/entity/registry.hpp>

#include "ecs/components/carried_items.hpp"
#include "ecs/components/container.hpp"
#include "ecs/components/item.hpp"
#include "ecs/components/item_stack.hpp"
#include "ecs/components/selectable.hpp"
#include "ecs/components/weared_items.hpp"
#include "ecs/components/wielded_items.hpp"
#include "world/world.hpp"

namespace dl::utils
{
std::vector<entt::entity> get_selected_entities(entt::registry& registry)
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

std::vector<entt::entity> get_entity_items(entt::registry& registry, entt::entity entity)
{
  std::vector<entt::entity> items{};

  if (registry.all_of<WieldedItems>(entity))
  {
    const auto& wielded_items = registry.get<WieldedItems>(entity);

    if (registry.valid(wielded_items.left_hand))
    {
      items.push_back(wielded_items.left_hand);
    }
    if (registry.valid(wielded_items.right_hand) && wielded_items.right_hand != wielded_items.left_hand)
    {
      items.push_back(wielded_items.right_hand);
    }
  }
  if (registry.all_of<WearedItems>(entity))
  {
    const auto& weared_items = registry.get<WearedItems>(entity);
    items.insert(items.end(), weared_items.items.begin(), weared_items.items.end());
  }
  if (registry.all_of<CarriedItems>(entity))
  {
    const auto& carried_items = registry.get<CarriedItems>(entity);
    items.insert(items.end(), carried_items.items.begin(), carried_items.items.end());
  }

  return items;
}

bool has_item(entt::registry& registry, entt::entity entity, entt::entity item)
{
  auto& wielded = registry.get<WieldedItems>(entity);

  if (wielded.left_hand == item)
  {
    return true;
  }
  else if (wielded.right_hand == item)
  {
    return true;
  }

  auto& carried = registry.get<CarriedItems>(entity);

  for (auto it = carried.items.begin(); it != carried.items.end(); ++it)
  {
    if (*it != item)
    {
      continue;
    }

    return true;
  }

  auto& weared = registry.get<WearedItems>(entity);

  for (auto it = weared.items.begin(); it != weared.items.end(); ++it)
  {
    if (*it == item)
    {
      return true;
    }
  }

  return false;
}

bool remove_item_from_entity(entt::registry& registry, entt::entity entity, entt::entity item)
{
  auto& wielded = registry.get<WieldedItems>(entity);

  if (wielded.left_hand == item)
  {
    wielded.left_hand = entt::null;
    return true;
  }
  else if (wielded.right_hand == item)
  {
    wielded.right_hand = entt::null;
    return true;
  }

  auto& carried = registry.get<CarriedItems>(entity);

  for (auto it = carried.items.begin(); it != carried.items.end(); ++it)
  {
    if (*it != item)
    {
      continue;
    }

    auto& item_component = registry.get<Item>(*it);
    // const auto& item_data = m_world.get_item_data(item_component.id);

    assert(registry.valid(item_component.container) && "Encountered carried item without container");

    // // TODO: Separate container weight and volume calculations to another function
    auto& container = registry.get<Container>(item_component.container);
    // container.weight_occupied -= item_data.weight;
    // container.volume_occupied -= item_data.volume;

    item_component.container = entt::null;
    container.items.erase(std::find(container.items.begin(), container.items.end(), *it));
    carried.items.erase(it);
    return true;
  }

  auto& weared = registry.get<WearedItems>(entity);

  for (auto it = weared.items.begin(); it != weared.items.end(); ++it)
  {
    if (*it == item)
    {
      weared.items.erase(it);
      return true;
    }
  }

  return false;
}

void decrease_container_weight_and_volume_by_item(World& world, entt::registry& registry, entt::entity entity, entt::entity item, const int quantity)
{
  assert(registry.valid(item) && "Invalid item entity");

  auto& item_component = registry.get<Item>(item);

  if (!registry.valid(item_component.container))
  {
    return;
  }

  int quantity_to_remove = quantity;

  if (quantity_to_remove == -1 && registry.all_of<ItemStack>(item))
  {
    auto& item_stack = registry.get<ItemStack>(item);
    quantity_to_remove = item_stack.quantity;
  }
  else if (quantity_to_remove == -1)
  {
    quantity_to_remove = 1;
  }

  const auto& item_data = world.get_item_data(item_component.id);

  const auto current_container = item_component.container;

  while (current_container != entt::null && registry.valid(current_container))
  {
    auto& container = registry.get<Container>(current_container);
    container.weight_occupied -= item_data.weight * quantity_to_remove;
    container.volume_occupied -= item_data.volume * quantity_to_remove;
  }
}
}  // namespace dl::utils
