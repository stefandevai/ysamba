#include "./player_controls.hpp"

#include <spdlog/spdlog.h>

#include <entt/core/hashed_string.hpp>

#include "ai/actions/walk.hpp"
#include "core/events/emitter.hpp"
#include "core/events/game.hpp"
#include "core/maths/random.hpp"
#include "ecs/components/action_walk.hpp"
#include "ecs/components/job_data.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/society_agent.hpp"
#include "ecs/components/sound_effect.hpp"
#include "world/society/job.hpp"
#include "world/target.hpp"

namespace dl
{
void walk(entt::registry& registry, SocietyAgent& agent, const Vector3 target)
{
  // Cancel all previous jobs if player moves
  if (!agent.jobs.empty())
  {
    agent.clear_jobs(registry);
  }

  action::walk::job({
      .registry = registry,
      .agent = &agent,
      .position = target,
  });
}

using namespace entt::literals;

const std::vector<uint32_t> footstep_sounds_ids{
    "sound_effect_footstep_dirt_00"_hs,
    "sound_effect_footstep_dirt_01"_hs,
    "sound_effect_footstep_dirt_02"_hs,
    "sound_effect_footstep_dirt_03"_hs,
};

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
    const auto sound_id = random::select<uint32_t>(footstep_sounds_ids);
    auto footsteps_sound = registry.create();
    registry.emplace<SoundEffect>(footsteps_sound, sound_id);

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
