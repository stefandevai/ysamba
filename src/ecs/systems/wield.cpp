#include "./wield.hpp"

#include <spdlog/spdlog.h>

#include "ecs/components/action_wield.hpp"
#include "ecs/components/biology.hpp"
#include "ecs/components/item.hpp"
#include "ecs/components/job_data.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/wielded_items.hpp"
#include "world/target.hpp"
#include "world/world.hpp"

namespace dl
{
const auto stop_wield = [](entt::registry& registry, const entt::entity entity, const entt::entity job)
{
  auto& job_data = registry.get<JobData>(job);
  job_data.status = JobStatus::Finished;
  registry.remove<ActionWield>(entity);
};

WieldSystem::WieldSystem(World& world) : m_world(world) {}

void WieldSystem::update(entt::registry& registry)
{
  auto view = registry.view<ActionWield, WieldedItems, const Position>();
  for (const auto entity : view)
  {
    auto& action_wield = registry.get<ActionWield>(entity);
    const auto& target = registry.get<Target>(action_wield.job);
    const entt::entity item = static_cast<entt::entity>(target.id);

    if (!registry.valid(item))
    {
      stop_wield(registry, entity, action_wield.job);
      continue;
    }

    // Check if target tile is still there
    if (!m_world.spatial_hash.has(item, target.position.x, target.position.y, target.position.z))
    {
      stop_wield(registry, entity, action_wield.job);
      continue;
    }

    if (!registry.all_of<WieldedItems>(entity))
    {
      spdlog::warn("Trying to wield object in an entity without WieldedItems component");
      stop_wield(registry, entity, action_wield.job);
      continue;
    }

    auto& item_component = registry.get<Item>(item);
    auto& wielded_items = registry.get<WieldedItems>(entity);

    if (!registry.valid(wielded_items.right_hand))
    {
      wielded_items.right_hand = item;
      registry.remove<Position>(item);
      registry.remove<Sprite>(item);
    }
    else if (!registry.valid(wielded_items.left_hand))
    {
      wielded_items.left_hand = item;
      registry.remove<Position>(item);
      registry.remove<Sprite>(item);
    }
    else
    {
      // TODO: Handle both hands occupied
      const auto& item_data = m_world.get_item_data(item_component.id);
      spdlog::warn("Trying to wield {} when both hands are occupied", item_data.name);
    }

    stop_wield(registry, entity, action_wield.job);
  }
}

}  // namespace dl
