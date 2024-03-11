#include "./walk.hpp"

#include <spdlog/spdlog.h>

#include "config.hpp"
#include "core/maths/vector.hpp"
#include "ecs/components/action_walk.hpp"
#include "ecs/components/job_data.hpp"
#include "ecs/components/movement.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/walk_path.hpp"
#include "world/target.hpp"
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
const auto stop_walk = [](entt::registry& registry, const entt::entity entity, const entt::entity job) {
  auto& job_data = registry.get<JobData>(job);
  job_data.status = JobStatus::Finished;
  registry.remove<ActionWalk>(entity);
  registry.remove<WalkPath>(entity);
  registry.remove<Movement>(entity);
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

    const auto& target = registry.get<Target>(action_walk.job);
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
      stop_walk(registry, entity, action_walk.job);
      continue;
    }

    if (!registry.all_of<WalkPath>(entity))
    {
      auto& walk_path = registry.emplace<WalkPath>(entity);
      walk_path.steps = m_world.find_path(
          Vector3i{std::round(position.x), std::round(position.y), std::round(position.z)}, target.position);
    }

    auto& walk_path = registry.get<WalkPath>(entity);

    // If we got to the desired distance from the target, stop walking
    if (std::abs(target.position.x - position.x) <= target.distance_offset
        && std::abs(target.position.y - position.y) <= target.distance_offset)
    {
      stop_walk(registry, entity, action_walk.job);
      continue;
    }

    // If there are no more steps in the path, stop walking
    if (walk_path.steps.empty())
    {
      stop_walk(registry, entity, action_walk.job);
      continue;
    }

    auto current_target_position = walk_path.steps.back();

    if (walk_path.steps.size() > 1 && std::abs(current_target_position.x - position.x) < 0.05
        && std::abs(current_target_position.y - position.y) < 0.05)
    {
      walk_path.steps.pop_back();
      current_target_position = walk_path.steps.back();
    }

    auto x_dir = current_target_position.x - position.x;
    auto y_dir = current_target_position.y - position.y;
    x_dir = x_dir > 0.0 ? 1.0 : x_dir < 0.0 ? -1.0 : 0.0;
    y_dir = y_dir > 0.0 ? 1.0 : y_dir < 0.0 ? -1.0 : 0.0;

    registry.patch<Movement>(entity, [x_dir, y_dir](auto& movement_component) {
      movement_component.direction.x = x_dir;
      movement_component.direction.y = y_dir;
    });
  }
}

}  // namespace dl
