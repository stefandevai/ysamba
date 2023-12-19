#include "./walk.hpp"

#include <spdlog/spdlog.h>

#include "ecs/components/action_walk.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/velocity.hpp"
#include "ecs/components/walk_path.hpp"
#include "world/world.hpp"

namespace dl
{
const auto stop_walk = [](entt::registry& registry, const entt::entity entity, const Job* job) {
  registry.remove<ActionWalk>(entity);
  registry.remove<WalkPath>(entity);
  job->status = JobStatus::Finished;
};

WalkSystem::WalkSystem(World& world) : m_world(world) {}

void WalkSystem::update(entt::registry& registry)
{
  auto view = registry.view<ActionWalk, const Position>();
  for (const auto entity : view)
  {
    auto& action_walk = registry.get<ActionWalk>(entity);
    const auto& job = action_walk.job;
    const auto& target = job->target;
    const auto& position = registry.get<Position>(entity);

    if (!registry.all_of<WalkPath>(entity))
    {
      auto& walk_path = registry.emplace<WalkPath>(entity);
      walk_path.steps = m_world.get_path_between(Vector3i{position.x, position.y, position.z}, target.position);
    }

    auto& walk_path = registry.get<WalkPath>(entity);

    // If the target is not adjacent, move towards the target
    if (std::abs(target.position.x - std::round(position.x)) > target.distance_offset ||
        std::abs(target.position.y - std::round(position.y - position.z)) > target.distance_offset)
    {
      // If the target path is empty, that means that the target disappeared.
      if (walk_path.steps.empty())
      {
        stop_walk(registry, entity, job);
        continue;
      }

      auto& current_target_position = walk_path.steps.top();

      if (std::round(position.x) == current_target_position.first &&
          std::round(position.y) == current_target_position.second)
      {
        walk_path.steps.pop();
        current_target_position = walk_path.steps.top();
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

    stop_walk(registry, entity, job);
  }
}

}  // namespace dl
