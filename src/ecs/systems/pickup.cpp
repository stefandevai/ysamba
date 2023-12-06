#include "./pickup.hpp"

#include <spdlog/spdlog.h>

#include "ecs/components/action_pickup.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/society_agent.hpp"
#include "ecs/components/velocity.hpp"
#include "ecs/components/weared_items.hpp"
#include "world/world.hpp"

namespace dl
{
auto stop_pickup = [](entt::registry& registry, const entt::entity entity, SocietyAgent& agent) {
  registry.remove<ActionPickup>(entity);
  agent.state = SocietyAgent::State::Idle;
  agent.jobs.top().status = JobStatus::Finished;
};

PickupSystem::PickupSystem(World& world) : m_world(world) {}

void PickupSystem::update(entt::registry& registry)
{
  auto view = registry.view<SocietyAgent, ActionPickup, const Position>();
  for (const auto entity : view)
  {
    auto& action_pickup = registry.get<ActionPickup>(entity);
    auto& agent = registry.get<SocietyAgent>(entity);

    if (!registry.valid(action_pickup.target.entity))
    {
      stop_pickup(registry, entity, agent);
      continue;
    }

    auto& target = action_pickup.target;

    // Check if target tile is still there
    if (!m_world.spatial_hash.has(target.entity, target.x, target.y))
    {
      stop_pickup(registry, entity, agent);
      continue;
    }

    registry.remove<Position>(target.entity);
    registry.remove<Visibility>(target.entity);

    auto& weared_items = registry.get<WearedItems>(entity);
    weared_items.items.push_back(target.entity);

    stop_pickup(registry, entity, agent);
  }
}

}  // namespace dl
