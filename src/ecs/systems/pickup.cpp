#include "./pickup.hpp"

#include <spdlog/spdlog.h>

#include "ecs/components/action_pickup.hpp"
#include "ecs/components/carried_items.hpp"
#include "ecs/components/position.hpp"
#include "world/world.hpp"

namespace dl
{
const auto stop_pickup = [](entt::registry& registry, const entt::entity entity, const Job* job) {
  registry.remove<ActionPickup>(entity);
  job->status = JobStatus::Finished;
};

PickupSystem::PickupSystem(World& world) : m_world(world) {}

void PickupSystem::update(entt::registry& registry)
{
  auto view = registry.view<ActionPickup, const Position>();
  for (const auto entity : view)
  {
    auto& action_pickup = registry.get<ActionPickup>(entity);
    const auto& job = action_pickup.job;
    const auto& target = job->target;
    const entt::entity item = static_cast<entt::entity>(target.id);

    if (!registry.valid(item))
    {
      stop_pickup(registry, entity, job);
      continue;
    }

    // Check if target tile is still there
    if (!m_world.spatial_hash.has(item, target.position.x, target.position.y))
    {
      stop_pickup(registry, entity, job);
      continue;
    }

    registry.remove<Position>(item);
    registry.remove<Visibility>(item);

    auto& carried_items = registry.get<CarriedItems>(entity);
    carried_items.items.push_back(item);

    stop_pickup(registry, entity, job);
  }
}

}  // namespace dl
