#include "./job.hpp"

#include <spdlog/spdlog.h>

#include "ecs/components/action_break.hpp"
#include "ecs/components/action_dig.hpp"
#include "ecs/components/action_harvest.hpp"
#include "ecs/components/action_pickup.hpp"
#include "ecs/components/action_walk.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/society_agent.hpp"

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

      switch (current_job.type)
      {
      case JobType::Walk:
        m_create_tile_action<ActionWalk>(entity, registry, current_job);
        break;
      case JobType::Harvest:
        m_create_tile_action<ActionHarvest>(entity, registry, current_job);
        break;
      case JobType::Break:
        m_create_tile_action<ActionBreak>(entity, registry, current_job);
        break;
      case JobType::Dig:
        m_create_tile_action<ActionDig>(entity, registry, current_job);
        break;
      case JobType::Pickup:
        m_create_item_action<ActionPickup>(entity, registry, current_job);
        break;
      }
    }
    else if (current_job.status == JobStatus::Finished)
    {
      agent.jobs.pop();
    }
  }
}

}  // namespace dl
