#include "./player_controls.hpp"

#include <spdlog/spdlog.h>

#include <entt/core/hashed_string.hpp>

#include "ecs/components/action_walk.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/society_agent.hpp"
#include "world/society/job.hpp"
#include "world/target.hpp"

namespace dl
{
PlayerControlsSystem::PlayerControlsSystem() {}

void PlayerControlsSystem::update(entt::registry& registry)
{
  using namespace entt::literals;

  auto view = registry.view<entt::tag<"player_controls"_hs>>();
  for (const auto entity : view)
  {
    if (!m_input_manager.is_context("player_controls"_hs))
    {
      m_input_manager.push_context("player_controls"_hs);
    }

    // if (registry.all_of<ActionWalk>(entity))
    // {
    //   continue;
    // }

    // auto& agent = registry.get<SocietyAgent>(entity);
    auto& position = registry.get<Position>(entity);

    if (m_input_manager.poll_action("move_left"_hs))
    {
      position.x -= 1;
      // agent.jobs.push(Job{JobType::Walk, 0, Target{{position.x - 1, position.y, position.z}, 0, 0}});
    }
    else if (m_input_manager.poll_action("move_top"_hs))
    {
      position.y -= 1;
      // agent.jobs.push(Job{JobType::Walk, 0, Target{{position.x, position.y - 1, position.z}, 0, 0}});
    }
    else if (m_input_manager.poll_action("move_right"_hs))
    {
      position.x += 1;
      // agent.jobs.push(Job{JobType::Walk, 0, Target{{position.x + 1, position.y, position.z}, 0, 0}});
    }
    else if (m_input_manager.poll_action("move_bottom"_hs))
    {
      position.y += 1;
      // agent.jobs.push(Job{JobType::Walk, 0, Target{{position.x, position.y + 1, position.z}, 0, 0}});
    }
    else if (m_input_manager.poll_action("move_top_left"_hs))
    {
      position.y -= 1;
      position.x -= 1;
      // agent.jobs.push(Job{JobType::Walk, 0, Target{{position.x - 1, position.y - 1, position.z}, 0, 0}});
    }
    else if (m_input_manager.poll_action("move_top_right"_hs))
    {
      position.y -= 1;
      position.x += 1;
      // agent.jobs.push(Job{JobType::Walk, 0, Target{{position.x + 1, position.y - 1, position.z}, 0, 0}});
    }
    else if (m_input_manager.poll_action("move_bottom_right"_hs))
    {
      position.y += 1;
      position.x += 1;
      // agent.jobs.push(Job{JobType::Walk, 0, Target{{position.x + 1, position.y + 1, position.z}, 0, 0}});
    }
    else if (m_input_manager.poll_action("move_bottom_left"_hs))
    {
      position.y += 1;
      position.x -= 1;
      // agent.jobs.push(Job{JobType::Walk, 0, Target{{position.x - 1, position.y + 1, position.z}, 0, 0}});
    }
  }
}

}  // namespace dl
