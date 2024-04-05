#include "./pickup.hpp"

#include <spdlog/spdlog.h>

#include "ecs/components/action_pickup.hpp"
#include "ecs/components/carried_items.hpp"
#include "ecs/components/container.hpp"
#include "ecs/components/item.hpp"
#include "ecs/components/job_data.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/sprite.hpp"
#include "ecs/components/weared_items.hpp"
#include "ecs/components/wielded_items.hpp"
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
  auto view = registry.view<ActionPickup, const Position>();
  for (const auto entity : view)
  {
    auto& action_pickup = registry.get<ActionPickup>(entity);
    const auto& target = registry.get<Target>(action_pickup.job);
    const entt::entity item = static_cast<entt::entity>(target.id);

    if (!registry.valid(item))
    {
      stop_pickup(registry, entity, action_pickup.job);
      continue;
    }

    // Check if target tile is still there
    if (!m_world.spatial_hash.has(item, target.position.x, target.position.y, target.position.z))
    {
      stop_pickup(registry, entity, action_pickup.job);
      continue;
    }

    // Check if we still have the capacity to pickup
    auto& item_component = registry.get<Item>(item);
    const auto& item_data = m_world.get_item_data(item_component.id);
    const auto container_entity = get_container_with_enough_capacity(registry, entity, item_data);

    if (!registry.valid(container_entity))
    {
      stop_pickup(registry, entity, action_pickup.job);
      continue;
    }

    // Wield item
    if (registry.all_of<WieldedItems>(container_entity))
    {
      auto& wielded = registry.get<WieldedItems>(entity);

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
        continue;
      }
    }
    // Put it in a container
    else if (registry.all_of<Container>(container_entity))
    {
      auto& container = registry.get<Container>(container_entity);
      container.weight_occupied += item_data.weight;
      container.volume_occupied += item_data.volume;
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
      continue;
    }

    registry.remove<Position>(item);
    registry.remove<Sprite>(item);
    stop_pickup(registry, entity, action_pickup.job);
  }
}

bool PickupSystem::can_pickup(entt::registry& registry, entt::entity entity, const ItemData& item_data)
{
  return registry.valid(get_container_with_enough_capacity(registry, entity, item_data));
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

        // TODO: Check materials that the container can carry
        if (container.volume_occupied + item_data.volume <= container.volume_capacity
            && container.weight_occupied + item_data.weight <= container.weight_capacity)
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

      // TODO: Check materials that the container can carry
      if (container.volume_occupied + item_data.volume <= container.volume_capacity
          && container.weight_occupied + item_data.weight <= container.weight_capacity)
      {
        return item;
      }
    }
  }

  return entt::null;
}

entt::entity PickupSystem::get_container_with_enough_capacity(entt::registry& registry,
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

    // Left hand is empty
    if (!registry.valid(wielded.left_hand))
    {
      return entity;
    }
    // Right hand is empty
    else if (!registry.valid(wielded.right_hand))
    {
      return entity;
    }
  }

  return entt::null;
}

}  // namespace dl
