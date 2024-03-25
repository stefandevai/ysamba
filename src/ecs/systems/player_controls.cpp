#include "./player_controls.hpp"

#include <spdlog/spdlog.h>

#include <entt/core/hashed_string.hpp>

#include "core/events/emitter.hpp"
#include "core/events/game.hpp"
#include "ecs/components/action_walk.hpp"
#include "ecs/components/job_data.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/society_agent.hpp"
#include "world/society/job.hpp"
#include "world/target.hpp"

namespace dl
{
void walk(entt::registry& registry, SocietyAgent& agent, const Vector3 target)
{
  const auto job_entity = registry.create();
  registry.emplace<Target>(job_entity, std::move(target), 0, 0);
  registry.emplace<JobData>(job_entity, JobType::Walk);

  // // Cancel all previous jobs if player moves
  // if (!agent.jobs.empty())
  // {
  //   agent.jobs.clear();
  // }

  agent.jobs.push(Job{0, job_entity});
}

PlayerControlsSystem::PlayerControlsSystem(EventEmitter& event_emitter) : m_event_emitter(event_emitter) {}

void PlayerControlsSystem::update(entt::registry& registry, const entt::entity player)
{
  using namespace entt::literals;

  if (!m_input_manager.is_context("gameplay"_hs))
  {
    return;
  }

  auto& agent = registry.get<SocietyAgent>(player);
  auto& position = registry.get<Position>(player);
  bool walked = false;

  if (m_input_manager.poll_action("move_left"_hs))
  {
    walk(registry, agent, {std::round(position.x) - 1.0, position.y, position.z});
    walked = true;
  }
  else if (m_input_manager.poll_action("move_top"_hs))
  {
    walk(registry, agent, {position.x, std::round(position.y) - 1.0, position.z});
    walked = true;
  }
  else if (m_input_manager.poll_action("move_right"_hs))
  {
    walk(registry, agent, {std::round(position.x + 1.0), position.y, position.z});
    walked = true;
  }
  else if (m_input_manager.poll_action("move_bottom"_hs))
  {
    walk(registry, agent, {position.x, std::round(position.y + 1.0), position.z});
    walked = true;
  }
  else if (m_input_manager.poll_action("move_top_left"_hs))
  {
    walk(registry, agent, {std::round(position.x) - 1.0, std::round(position.y - 1.0), position.z});
    walked = true;
  }
  else if (m_input_manager.poll_action("move_top_right"_hs))
  {
    walk(registry, agent, {std::round(position.x) + 1.0, std::round(position.y - 1.0), position.z});
    walked = true;
  }
  else if (m_input_manager.poll_action("move_bottom_right"_hs))
  {
    walk(registry, agent, {std::round(position.x) + 1.0, std::round(position.y + 1.0), position.z});
    walked = true;
  }
  else if (m_input_manager.poll_action("move_bottom_left"_hs))
  {
    walk(registry, agent, {std::round(position.x) - 1.0, std::round(position.y + 1.0), position.z});
    walked = true;
  }

  // Update game until all jobs are done
  if (walked)
  {
    while (!agent.jobs.empty())
    {
      m_event_emitter.publish(UpdateGameEvent{});
    }
  }
  // Update a single frame
  else if (!agent.jobs.empty())
  {
    m_event_emitter.publish(UpdateGameEvent{});
  }
}

}  // namespace dl
