#include "./job.hpp"

#include <spdlog/spdlog.h>

#include "ecs/components/action_pickup.hpp"
#include "ecs/components/action_walk.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/society_agent.hpp"
#include "graphics/constants.hpp"
#include "graphics/frame_data_types.hpp"

namespace dl
{
JobSystem::JobSystem(World& world) : m_world(world) {}

void JobSystem::update(entt::registry& registry, const double delta)
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
      case JobType::Pickup:
        m_create_item_action<ActionPickup>(entity, registry, current_job);
        break;
      default:
        break;
      }
    }
    else if (current_job.status == JobStatus::InProgress)
    {
      switch (current_job.type)
      {
      case JobType::Harvest:
      case JobType::Break:
      case JobType::Dig:
        m_update_tile_job(current_job, delta, registry);
      default:
        break;
      }
    }
    else if (current_job.status == JobStatus::Finished)
    {
      agent.jobs.pop();
    }
  }
}

void JobSystem::m_update_tile_job(const Job& job, const double delta, entt::registry& registry)
{
  if (job.status != JobStatus::InProgress)
  {
    return;
  }

  job.time_left -= delta;

  if (job.time_left < 0.0)
  {
    job.status = JobStatus::Finished;

    const auto& tile = m_world.get(job.target.position.x, job.target.position.y, job.target.position.z);

    // Check if target tile is still there
    if (tile.id != job.target.id)
    {
      return;
    }

    const auto& tile_data = m_world.get_tile_data(tile.id);
    const auto& action = tile_data.actions.at(job.type);

    m_world.replace(tile.id, action.turns_into, job.target.position.x, job.target.position.y, job.target.position.z);

    if (action.gives.empty())
    {
      return;
    }

    for (const auto& item : action.gives)
    {
      const auto drop = registry.create();
      registry.emplace<Position>(drop, job.target.position.x, job.target.position.y, job.target.position.z);
      registry.emplace<Visibility>(drop,
                                   m_world.get_texture_id(),
                                   item.first,
                                   frame_data_type::item,
                                   job.target.position.z + renderer::layer_z_offset_items);
      registry.emplace<Pickable>(drop, item.first);
    }
  }
}

}  // namespace dl
