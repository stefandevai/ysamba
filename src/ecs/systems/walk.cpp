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

    spdlog::warn("WAAAAAAAAAAAAAAAALK");
    spdlog::debug("Pos {} {}", position.x, position.y);
    spdlog::debug("Target {} {}", target.position.x, target.position.y);
    spdlog::debug("Dir {} {}", movement.direction.x, movement.direction.y);

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

    spdlog::debug("OFFSET {} {} {} {} {}",
                  std::abs(target.position.x - position.x),
                  std::abs(target.position.y - position.y),
                  target.distance_offset,
                  position.x,
                  position.y);

    // If the target is not adjacent, move towards the target
    if (std::abs(target.position.x - position.x) > target.distance_offset + 0.05
        || std::abs(target.position.y - position.y) > target.distance_offset + 0.05)
    {
      // If the target path is empty, that means that the target disappeared.
      if (walk_path.steps.empty())
      {
        stop_walk(registry, entity, action_walk.job);
        continue;
      }

      auto& current_target_position = walk_path.steps.back();

      // spdlog::debug("TARGET {} {}", current_target_position.x, current_target_position.y);

      // if (std::round(position.x) == current_target_position.x && std::round(position.y) == current_target_position.y
      // && walk_path.steps.size() > 1)
      if (walk_path.steps.size() > 1 && std::abs(current_target_position.x - position.x) < 0.05
          && std::abs(current_target_position.y - position.y) < 0.05)
      {
        walk_path.steps.pop_back();
        current_target_position = walk_path.steps.back();
        // spdlog::debug("TARGET {} {}", current_target_position.x, current_target_position.y);
      }
      // else if (std::round(position.x) == current_target_position.x && std::round(position.y) ==
      // current_target_position.y && walk_path.steps.size() == 1)
      // {
      //   walk_path.steps.pop_back();
      //   spdlog::debug("TARGET LAST {} {}", current_target_position.x, current_target_position.y);
      // }

      auto x_dir = current_target_position.x - position.x;
      auto y_dir = current_target_position.y - position.y;

      // spdlog::debug("BEF TAR {} {}", current_target_position.x, current_target_position.y);
      spdlog::debug("NEW DIRECTION {} {}", x_dir, y_dir);

      if (x_dir == 0.0 && y_dir == 0.0)
      {
        walk_path.steps.pop_back();
        continue;
      }

      // x_dir = x_dir > 0.0 ? 1.0 : x_dir < 0.0 ? -1.0 : 0.0;
      // y_dir = y_dir > 0.0 ? 1.0 : y_dir < 0.0 ? -1.0 : 0.0;

      registry.patch<Movement>(entity, [x_dir, y_dir](auto& movement_component) {
        movement_component.direction.x = x_dir;
        movement_component.direction.y = y_dir;
      });

      continue;
    }

    // registry.patch<Position>(entity, [](auto& position)
    //     {
    //       position.x = std::round(position.x);
    //       position.y = std::round(position.y);
    //       position.z = std::round(position.z);
    //     });

    stop_walk(registry, entity, action_walk.job);
  }
}

}  // namespace dl
