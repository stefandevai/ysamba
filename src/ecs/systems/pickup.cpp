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

    const auto& position = registry.get<Position>(entity);

    auto& target = action_pickup.target;

    // If the target tile is not adjacent, move towards the target
    if (std::abs(target.x - std::round(position.x)) > 1 || std::abs(target.y - std::round(position.y)) > 1)
    {
      // If the target path is empty, that means that the target disappeared.
      if (target.path.size() <= 1)
      {
        stop_pickup(registry, entity, agent);
        continue;
      }
      auto current_target_position = target.path.top();

      if (std::round(position.x) == current_target_position.first &&
          std::round(position.y) == current_target_position.second)
      {
        target.path.pop();
        current_target_position = target.path.top();
      }

      const auto x_dir = current_target_position.first - std::round(position.x);
      const auto y_dir = current_target_position.second - std::round(position.y);

      if (registry.all_of<Velocity>(entity))
      {
        registry.patch<Velocity>(entity, [x_dir, y_dir](auto& velocity) {
          velocity.x = x_dir;
          velocity.y = y_dir;
        });
      }
      else
      {
        registry.emplace<Velocity>(entity, x_dir, y_dir, 0.);
      }
      continue;
    }

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
