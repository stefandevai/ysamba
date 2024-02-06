#include "./player_controls.hpp"

#include <spdlog/spdlog.h>

#include <entt/core/hashed_string.hpp>

#include "core/events/emitter.hpp"
#include "core/events/game.hpp"
#include "ecs/components/action_walk.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/society_agent.hpp"
#include "world/society/job.hpp"
#include "world/target.hpp"

namespace dl
{
PlayerControlsSystem::PlayerControlsSystem(EventEmitter& event_emitter) : m_event_emitter(event_emitter) {}

void PlayerControlsSystem::update(entt::registry& registry, const entt::entity player)
{
  using namespace entt::literals;

  if (!m_input_manager.is_context("gameplay"_hs))
  {
    return;
  }

  if (registry.all_of<ActionWalk>(player))
  {
    return;
  }

  auto& agent = registry.get<SocietyAgent>(player);
  auto& position = registry.get<Position>(player);

  if (m_input_manager.poll_action("move_left"_hs))
  {
    agent.jobs.push(Job{JobType::Walk, 0, Target{{position.x - 1, position.y, position.z}, 0, 0}});
  }
  else if (m_input_manager.poll_action("move_top"_hs))
  {
    agent.jobs.push(Job{JobType::Walk, 0, Target{{position.x, position.y - 1, position.z}, 0, 0}});
  }
  else if (m_input_manager.poll_action("move_right"_hs))
  {
    agent.jobs.push(Job{JobType::Walk, 0, Target{{position.x + 1, position.y, position.z}, 0, 0}});
  }
  else if (m_input_manager.poll_action("move_bottom"_hs))
  {
    agent.jobs.push(Job{JobType::Walk, 0, Target{{position.x, position.y + 1, position.z}, 0, 0}});
  }
  else if (m_input_manager.poll_action("move_top_left"_hs))
  {
    agent.jobs.push(Job{JobType::Walk, 0, Target{{position.x - 1, position.y - 1, position.z}, 0, 0}});
  }
  else if (m_input_manager.poll_action("move_top_right"_hs))
  {
    agent.jobs.push(Job{JobType::Walk, 0, Target{{position.x + 1, position.y - 1, position.z}, 0, 0}});
  }
  else if (m_input_manager.poll_action("move_bottom_right"_hs))
  {
    agent.jobs.push(Job{JobType::Walk, 0, Target{{position.x + 1, position.y + 1, position.z}, 0, 0}});
  }
  else if (m_input_manager.poll_action("move_bottom_left"_hs))
  {
    agent.jobs.push(Job{JobType::Walk, 0, Target{{position.x - 1, position.y + 1, position.z}, 0, 0}});
  }

  // Update game until all jobs are done
  while (!agent.jobs.empty())
  {
    m_event_emitter.publish(UpdateGameEvent{});
  }
}

}  // namespace dl
