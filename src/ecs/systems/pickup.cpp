#include "./pickup.hpp"

#include <spdlog/spdlog.h>

#include "ecs/components/action_pickup.hpp"
#include "ecs/components/carried_items.hpp"
#include "ecs/components/container.hpp"
#include "ecs/components/item.hpp"
#include "ecs/components/item_stack.hpp"
#include "ecs/components/job_data.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/sprite.hpp"
#include "ecs/components/weared_items.hpp"
#include "ecs/components/wielded_items.hpp"
#include "world/item_factory.hpp"
#include "world/target.hpp"
#include "world/world.hpp"

namespace dl
{
const auto stop_pickup = [](entt::registry& registry, const entt::entity entity, const entt::entity job)
{
  auto& job_data = registry.get<JobData>(job);
  job_data.status = JobStatus::Finished;
  registry.remove<ActionPickup>(entity);
};

PickupSystem::PickupSystem(World& world) : m_world(world) {}

void PickupSystem::update(entt::registry& registry)
{
  auto pickup_view = registry.view<ActionPickup, const Position>();

  for (const auto entity : pickup_view)
  {
    const auto& action_pickup = registry.get<ActionPickup>(entity);
    const auto& job_data = registry.get<JobData>(action_pickup.job);

    if (job_data.type == JobType::Pickup)
    {
      m_pickup(registry, entity);
    }
    else if (job_data.type == JobType::PickupLiquid)
    {
      m_pickup_liquid(registry, entity);
    }
  }
}

bool PickupSystem::can_pickup(entt::registry& registry, entt::entity entity, const ItemData& item_data)
{
  return registry.valid(get_container(registry, entity, item_data));
}

entt::entity PickupSystem::iterate_containers(entt::registry& registry,
                                              const ItemData& item_data,
                                              const std::vector<entt::entity>& items)
{
  std::vector<const std::vector<entt::entity>*> item_stack = {&items};

  while (!item_stack.empty())
  {
    const auto current_items = item_stack.back();
    item_stack.pop_back();

    for (auto item : *current_items)
    {
      if (registry.all_of<Container>(item))
      {
        const auto& container = registry.get<Container>(item);
        const auto& matter_states = container.matter_states;
        const bool can_carry_item
            = std::find(matter_states.begin(), matter_states.end(), item_data.matter_state) != matter_states.end();
        bool has_capacity = false;

        // Ensure that the container has enough capacity to carry the item
        if (can_carry_item)
        {
          switch (item_data.matter_state)
          {
          case MatterState::Solid:
          {
            has_capacity = container.volume_occupied + item_data.volume <= container.volume_capacity
                           && container.weight_occupied + item_data.weight <= container.weight_capacity;
            break;
          }

          // The container needs to be empty to carry gas, plasma or liquid
          case MatterState::Gas:
          case MatterState::Plasma:
          case MatterState::Liquid:
          {
            has_capacity = container.volume_occupied == 0 && container.weight_occupied == 0;
            break;
          }
          default:
          {
            break;
          }
          }
        }

        if (has_capacity)
        {
          return item;
        }
        else
        {
          item_stack.push_back(&container.items);
        }
      }
    }
  }

  return entt::null;
};

entt::entity PickupSystem::search_container(entt::registry& registry,
                                            const ItemData& item_data,
                                            const std::vector<entt::entity>& items)
{
  for (const auto item : items)
  {
    if (registry.all_of<Container>(item))
    {
      const auto& container = registry.get<Container>(item);
      const auto& matter_states = container.matter_states;
      const bool can_carry_item
          = std::find(matter_states.begin(), matter_states.end(), item_data.matter_state) != matter_states.end();
      bool has_capacity = false;

      // Ensure that the container has enough capacity to carry the item
      if (can_carry_item)
      {
        switch (item_data.matter_state)
        {
        case MatterState::Solid:
        {
          has_capacity = container.volume_occupied + item_data.volume <= container.volume_capacity
                         && container.weight_occupied + item_data.weight <= container.weight_capacity;
          break;
        }

        // The container needs to be empty to carry gas, plasma or liquid
        case MatterState::Gas:
        case MatterState::Plasma:
        case MatterState::Liquid:
        {
          has_capacity = container.volume_occupied == 0 && container.weight_occupied == 0;
          break;
        }
        default:
        {
          break;
        }
        }
      }

      if (has_capacity)
      {
        return item;
      }
    }
  }

  return entt::null;
}

entt::entity PickupSystem::get_container(entt::registry& registry, entt::entity entity, const ItemData& item_data)
{
  if (item_data.flags.contains("LIQUID"))
  {
    return get_liquid_container(registry, entity, item_data);
  }

  if (registry.all_of<WearedItems>(entity))
  {
    const auto& weared = registry.get<WearedItems>(entity);
    const auto container = search_container(registry, item_data, weared.items);

    if (registry.valid(container))
    {
      return container;
    }
  }
  if (registry.all_of<CarriedItems>(entity))
  {
    const auto& carried = registry.get<CarriedItems>(entity);
    const auto container = search_container(registry, item_data, carried.items);

    if (registry.valid(container))
    {
      return container;
    }
  }
  if (registry.all_of<WieldedItems>(entity))
  {
    const auto& wielded = registry.get<WieldedItems>(entity);

    // Left hand is empty
    if (!registry.valid(wielded.left_hand))
    {
      return entity;
    }
    // Check if stackable item is already in left hand
    else if (registry.all_of<ItemStack, Item>(wielded.left_hand))
    {
      const auto left_item = registry.get<Item>(wielded.left_hand);

      if (left_item.id == item_data.id)
      {
        return entity;
      }
    }

    // Right hand is empty
    if (!registry.valid(wielded.right_hand))
    {
      return entity;
    }
    // Check if stackable item is already in right hand
    else if (registry.all_of<ItemStack, Item>(wielded.right_hand))
    {
      const auto right_item = registry.get<Item>(wielded.right_hand);

      if (right_item.id == item_data.id)
      {
        return entity;
      }
    }
  }

  return entt::null;
}

entt::entity PickupSystem::get_liquid_container(entt::registry& registry,
                                                entt::entity entity,
                                                const ItemData& item_data)
{
  if (registry.all_of<WearedItems>(entity))
  {
    const auto& weared = registry.get<WearedItems>(entity);
    const auto container = search_container(registry, item_data, weared.items);

    if (registry.valid(container))
    {
      return container;
    }
  }
  if (registry.all_of<CarriedItems>(entity))
  {
    const auto& carried = registry.get<CarriedItems>(entity);
    const auto container = search_container(registry, item_data, carried.items);

    if (registry.valid(container))
    {
      return container;
    }
  }
  if (registry.all_of<WieldedItems>(entity))
  {
    const auto& wielded = registry.get<WieldedItems>(entity);

    if (registry.all_of<Container>(wielded.left_hand))
    {
      const auto& container = registry.get<Container>(wielded.left_hand);
      const auto& matter_states = container.matter_states;
      const bool can_carry_liquid
          = std::find(matter_states.begin(), matter_states.end(), MatterState::Liquid) != matter_states.end();

      if (can_carry_liquid && container.volume_occupied == 0 && container.weight_occupied == 0)
      {
        return wielded.left_hand;
      }
    }
    else if (registry.all_of<Container>(wielded.right_hand))
    {
      const auto& container = registry.get<Container>(wielded.right_hand);
      const auto& matter_states = container.matter_states;
      const bool can_carry_liquid
          = std::find(matter_states.begin(), matter_states.end(), MatterState::Liquid) != matter_states.end();

      if (can_carry_liquid && container.volume_occupied == 0 && container.weight_occupied == 0)
      {
        return wielded.right_hand;
      }
    }
  }

  return entt::null;
}

void PickupSystem::m_pickup(entt::registry& registry, entt::entity entity)
{
  const auto& action_pickup = registry.get<ActionPickup>(entity);
  const auto& target = registry.get<Target>(action_pickup.job);
  const entt::entity item = static_cast<entt::entity>(target.id);

  if (!registry.valid(item))
  {
    stop_pickup(registry, entity, action_pickup.job);
    return;
  }

  // Check if target tile is still there
  if (!m_world.spatial_hash.has(item, target.position.x, target.position.y, target.position.z))
  {
    stop_pickup(registry, entity, action_pickup.job);
    return;
  }

  auto& item_component = registry.get<Item>(item);

  // Check if we still have the capacity to pickup
  const auto& item_data = m_world.get_item_data(item_component.id);
  const auto container_entity = get_container(registry, entity, item_data);

  if (!registry.valid(container_entity))
  {
    stop_pickup(registry, entity, action_pickup.job);
    return;
  }

  // Wield item
  if (registry.all_of<WieldedItems>(container_entity))
  {
    auto& wielded = registry.get<WieldedItems>(entity);
    entt::entity existing_item = entt::null;

    // Add to stack if item is already in wield slot
    if (registry.valid(wielded.left_hand) && registry.all_of<ItemStack, Item>(wielded.left_hand))
    {
      const auto left_item = registry.get<Item>(wielded.left_hand);

      if (left_item.id == item_data.id)
      {
        existing_item = wielded.left_hand;
      }
    }
    else if (registry.valid(wielded.right_hand) && registry.all_of<ItemStack, Item>(wielded.right_hand))
    {
      const auto right_item = registry.get<Item>(wielded.right_hand);

      if (right_item.id == item_data.id)
      {
        existing_item = wielded.right_hand;
      }
    }

    if (existing_item != entt::null)
    {
      auto& item_stack = registry.get<ItemStack>(item);
      auto& existing_stack = registry.get<ItemStack>(existing_item);
      existing_stack.quantity += item_stack.quantity;
      registry.destroy(item);
      stop_pickup(registry, entity, action_pickup.job);
      return;
    }

    // Wield item in free hand
    // TODO: Take into consideration if agent is left or right handed
    if (!registry.valid(wielded.left_hand))
    {
      wielded.left_hand = item;
    }
    else if (!registry.valid(wielded.right_hand))
    {
      wielded.right_hand = item;
    }
    else
    {
      // Should not get here
      spdlog::warn("Could not pickup item in wield slot");
      stop_pickup(registry, entity, action_pickup.job);
      return;
    }
  }
  // Put it in a container
  else if (registry.all_of<Container>(container_entity))
  {
    auto& container = registry.get<Container>(container_entity);
    container.weight_occupied += item_data.weight;
    container.volume_occupied += item_data.volume;

    // Check if item already exists in container and add to stack if it does
    if (registry.all_of<ItemStack>(item))
    {
      auto existing_item = std::find_if(container.items.begin(),
                                        container.items.end(),
                                        [&registry, &item_component](const auto& item)
                                        { return registry.get<Item>(item).id == item_component.id; });

      if (existing_item != container.items.end())
      {
        const auto& item_stack = registry.get<ItemStack>(item);
        auto& existing_item_stack = registry.get<ItemStack>(*existing_item);
        existing_item_stack.quantity += item_stack.quantity;
        registry.destroy(item);
        stop_pickup(registry, entity, action_pickup.job);
        return;
      }
    }

    // Add new item to container
    container.items.push_back(item);

    item_component.container = container_entity;

    // Duplicate item entity in CarriedItems component to
    // make item lookup easier
    // TODO: Implement binary tree with item id as index
    auto& carried_items = registry.get<CarriedItems>(entity);
    carried_items.items.push_back(item);
  }
  else
  {
    // Should not get here
    spdlog::warn("Could not pickup item in any slot");
    stop_pickup(registry, entity, action_pickup.job);
    return;
  }

  registry.remove<Position>(item);
  registry.remove<Sprite>(item);
  stop_pickup(registry, entity, action_pickup.job);
}

void PickupSystem::m_pickup_liquid(entt::registry& registry, entt::entity entity)
{
  const auto& action_pickup = registry.get<ActionPickup>(entity);
  const auto& target = registry.get<Target>(action_pickup.job);
  const auto& tile = m_world.get(target.position);
  const auto& action = tile.actions.at(JobType::PickupLiquid);

  assert(action.gives.size() == 1);
  const auto& item_data = m_world.get_item_data(action.gives[0].id);

  const auto container_entity = get_liquid_container(registry, entity, item_data);

  if (!registry.valid(container_entity))
  {
    stop_pickup(registry, entity, action_pickup.job);
    return;
  }

  // Create item entity
  const auto item = item_factory::create(item_data, registry);

  // // Add to container
  auto& item_component = registry.get<Item>(item);
  auto& item_stack = registry.get<ItemStack>(item);
  auto& container = registry.get<Container>(container_entity);

  assert(item_data.volume > 0.0);
  assert(item_data.weight > 0.0);

  const auto liquid_units_added_by_volume = std::floor(container.volume_capacity / item_data.volume);
  const auto liquid_units_added_by_weight = std::floor(container.weight_capacity / item_data.weight);
  const uint32_t liquid_units_added
      = static_cast<uint32_t>(std::min(liquid_units_added_by_volume, liquid_units_added_by_weight));

  item_stack.quantity = liquid_units_added;
  container.weight_occupied = item_data.weight * liquid_units_added;
  container.volume_occupied = item_data.volume * liquid_units_added;

  container.items.push_back(item);
  item_component.container = container_entity;

  // Duplicate item entity in CarriedItems component to
  // make item lookup easier
  // TODO: Implement binary tree with item id as index
  auto& carried_items = registry.get<CarriedItems>(entity);
  carried_items.items.push_back(item);

  stop_pickup(registry, entity, action_pickup.job);
}
}  // namespace dl
