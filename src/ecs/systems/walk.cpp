#include "./walk.hpp"

#include <spdlog/spdlog.h>

#include "config.hpp"
#include "core/maths/vector.hpp"
#include "ecs/components/action_walk.hpp"
#include "ecs/components/movement.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/walk_path.hpp"
#include "world/world.hpp"

namespace
{
void add_movement_component(entt::registry& registry, entt::entity entity)
{
  if (registry.all_of<dl::Movement>(entity))
  {
    return;
  }

  registry.emplace<dl::Movement>(entity);
}
}  // namespace

namespace dl
{
const auto stop_walk = [](entt::registry& registry, const entt::entity entity, const Job* job) {
  registry.remove<ActionWalk>(entity);
  registry.remove<WalkPath>(entity);
  registry.remove<Movement>(entity);

  assert(job != nullptr);
  job->status = JobStatus::Finished;
};

WalkSystem::WalkSystem(World& world, entt::registry& registry) : m_world(world)
{
  // Add a Movement component whenever a ActionWalk component is created
  registry.on_construct<ActionWalk>().connect<&add_movement_component>();
}

void WalkSystem::update(entt::registry& registry)
{
  auto view = registry.view<ActionWalk, Movement, const Position>();
  for (const auto entity : view)
  {
    auto& action_walk = registry.get<ActionWalk>(entity);
    const auto job = action_walk.job;

    assert(job != nullptr);
    const auto& target = job->target;
    const auto& position = registry.get<Position>(entity);
    auto& movement = registry.get<Movement>(entity);

    // If the entity collided, increment the retry counter
    if (movement.collided)
    {
      ++movement.retries;
    }
    // If there was no collision and the retry counter is greater than 0, reset it
    else if (movement.retries > 0)
    {
      movement.retries = 0;
    }

    // If the entity collided a certain number of times in a row, stop walking
    if (movement.retries > config::pathfinding::tries_after_collision)
    {
      stop_walk(registry, entity, job);
      continue;
    }

    if (!registry.all_of<WalkPath>(entity))
    {
      auto& walk_path = registry.emplace<WalkPath>(entity);
      walk_path.steps = m_world.find_path(Vector3i{position.x, position.y, position.z}, target.position);
    }

    auto& walk_path = registry.get<WalkPath>(entity);

    // If the target is not adjacent, move towards the target
    if (std::abs(target.position.x - std::round(position.x)) > target.distance_offset ||
        std::abs(target.position.y - std::round(position.y)) > target.distance_offset)
    {
      // If the target path is empty, that means that the target disappeared.
      if (walk_path.steps.empty())
      {
        stop_walk(registry, entity, job);
        continue;
      }

      auto& current_target_position = walk_path.steps.back();

      if (std::round(position.x) == current_target_position.x && std::round(position.y) == current_target_position.y)
      {
        walk_path.steps.pop_back();
        current_target_position = walk_path.steps.back();
      }

      const auto x_dir = current_target_position.x - std::round(position.x);
      const auto y_dir = current_target_position.y - std::round(position.y);

      registry.patch<Movement>(entity, [x_dir, y_dir](auto& movement_component) {
        movement_component.direction.x = x_dir;
        movement_component.direction.y = y_dir;
      });

      continue;
    }

    stop_walk(registry, entity, job);
  }
}

}  // namespace dl
