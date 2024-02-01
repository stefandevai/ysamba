#include "./wield.hpp"

#include <spdlog/spdlog.h>

#include "ecs/components/action_wield.hpp"
#include "ecs/components/biology.hpp"
#include "ecs/components/item.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/wielded_items.hpp"
#include "world/world.hpp"

namespace dl
{
const auto stop_wield = [](entt::registry& registry, const entt::entity entity, const Job* job) {
  registry.remove<ActionWield>(entity);
  job->status = JobStatus::Finished;
};

WieldSystem::WieldSystem(World& world) : m_world(world) {}

void WieldSystem::update(entt::registry& registry)
{
  auto view = registry.view<ActionWield, WieldedItems, const Position>();
  for (const auto entity : view)
  {
    auto& action_wield = registry.get<ActionWield>(entity);
    const auto& job = action_wield.job;
    const auto& target = job->target;
    const entt::entity item = static_cast<entt::entity>(target.id);

    if (!registry.valid(item))
    {
      stop_wield(registry, entity, job);
      continue;
    }

    // Check if target tile is still there
    if (!m_world.spatial_hash.has(item, target.position.x, target.position.y, target.position.z))
    {
      stop_wield(registry, entity, job);
      continue;
    }

    if (!registry.all_of<WieldedItems>(entity))
    {
      spdlog::warn("Trying to wield object in an entity without WieldedItems component");
      stop_wield(registry, entity, job);
      continue;
    }

    auto& item_component = registry.get<Item>(item);
    auto& wielded_items = registry.get<WieldedItems>(entity);

    if (!registry.valid(wielded_items.right_hand))
    {
      wielded_items.right_hand = item;
      registry.remove<Position>(item);
      registry.remove<Visibility>(item);
    }
    else if (!registry.valid(wielded_items.left_hand))
    {
      wielded_items.left_hand = item;
      registry.remove<Position>(item);
      registry.remove<Visibility>(item);
    }
    else
    {
      // TODO: Handle both hands occupied
      const auto& item_data = m_world.get_item_data(item_component.id);
      spdlog::warn("Trying to wield {} when both hands are occupied", item_data.name);
    }

    stop_wield(registry, entity, job);
  }
}

}  // namespace dl
