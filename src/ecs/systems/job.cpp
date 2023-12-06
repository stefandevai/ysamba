#include "./job.hpp"

#include <spdlog/spdlog.h>

#include "ecs/components/action_break.hpp"
#include "ecs/components/action_dig.hpp"
#include "ecs/components/action_harvest.hpp"
#include "ecs/components/action_walk.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/society_agent.hpp"
#include "world/society/job.hpp"
#include "world/world.hpp"

namespace dl
{
JobSystem::JobSystem(World& world) : m_world(world) {}

void JobSystem::update(entt::registry& registry)
{
  auto view = registry.view<SocietyAgent, const Position>();
  for (const auto entity : view)
  {
    auto& agent = registry.get<SocietyAgent>(entity);

    if (agent.jobs.empty())
    {
      continue;
    }

    auto& current_job = agent.jobs.top();

    if (current_job.status == JobStatus::Waiting)
    {
      current_job.status = JobStatus::InProgress;
      const auto& tile = m_world.get(current_job.target.x, current_job.target.y, current_job.target.z);
      const auto& position = registry.get<Position>(entity);
      const Vector3i tile_position = {std::round(position.x), std::round(position.y), std::round(position.z)};

      // Harvest
      if (current_job.id == 0)
      {
        auto& action_harvest = registry.emplace<ActionHarvest>(
            entity, TileTarget(tile.id, current_job.target.x, current_job.target.y, current_job.target.z));
        action_harvest.target.path = m_world.get_path_between(tile_position, current_job.target);
      }
      // Break
      else if (current_job.id == 1)
      {
        auto& action_break = registry.emplace<ActionBreak>(
            entity, TileTarget(tile.id, current_job.target.x, current_job.target.y, current_job.target.z));
        action_break.target.path = m_world.get_path_between(tile_position, current_job.target);
      }
      // Dig
      else if (current_job.id == 2)
      {
        auto& action_dig = registry.emplace<ActionDig>(
            entity, TileTarget(tile.id, current_job.target.x, current_job.target.y, current_job.target.z));
        action_dig.target.path = m_world.get_path_between(tile_position, current_job.target);
      }
      // Walk
      else if (current_job.id == 3)
      {
        auto& action_walk = registry.emplace_or_replace<ActionWalk>(
            entity, TileTarget(tile.id, current_job.target.x, current_job.target.y, current_job.target.z));
        action_walk.target.path = m_world.get_path_between(tile_position, current_job.target);
      }
      else
      {
        spdlog::critical("Unknown job id: {}", current_job.id);
      }
    }
  }
}

}  // namespace dl
