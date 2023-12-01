#include "./walk.hpp"

#include <spdlog/spdlog.h>

#include "ecs/components/action_walk.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/society_agent.hpp"
#include "ecs/components/velocity.hpp"
#include "ecs/components/weared_items.hpp"
#include "world/world.hpp"

namespace dl
{
auto stop_walk = [](entt::registry& registry, const entt::entity entity, SocietyAgent& agent) {
  registry.remove<ActionWalk>(entity);
  agent.state = SocietyAgent::State::Idle;
};

WalkSystem::WalkSystem(World& world) : m_world(world) {}

void WalkSystem::update(entt::registry& registry, const double delta)
{
  auto view = registry.view<SocietyAgent, ActionWalk, const Position>();
  for (const auto entity : view)
  {
    auto& action_walk = registry.get<ActionWalk>(entity);
    auto& agent = registry.get<SocietyAgent>(entity);

    if (!action_walk.target)
    {
      stop_walk(registry, entity, agent);
      continue;
    }

    const auto& position = registry.get<Position>(entity);

    auto& target = action_walk.target;

    // If the target tile is not adjacent, move towards the target
    if (std::abs(target.x - std::round(position.x)) > 1 || std::abs(target.y - std::round(position.y)) > 1)
    {
      // If the target path is empty, that means that the target disappeared.
      if (target.path.size() <= 1)
      {
        stop_walk(registry, entity, agent);
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

    stop_walk(registry, entity, agent);
  }
}

}  // namespace dl
