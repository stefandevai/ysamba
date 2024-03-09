#include "./job.hpp"

#include <spdlog/spdlog.h>

#include "constants.hpp"
#include "ecs/components/action_drop.hpp"
#include "ecs/components/action_pickup.hpp"
#include "ecs/components/action_walk.hpp"
#include "ecs/components/action_wear.hpp"
#include "ecs/components/action_wield.hpp"
#include "ecs/components/item.hpp"
#include "ecs/components/job_data.hpp"
#include "ecs/components/job_progress.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/society_agent.hpp"
#include "ecs/components/sprite.hpp"
#include "graphics/frame_data_types.hpp"

namespace
{
template <typename T>
void check_component(entt::registry& registry, entt::entity entity, const entt::entity job)
{
  if (!registry.all_of<T>(entity))
  {
    registry.emplace<T>(entity, job);
  }
}
}  // namespace

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
    auto& job_data = registry.get<JobData>(current_job.entity);

    if (job_data.status == JobStatus2::Waiting)
    {
      job_data.status = JobStatus2::InProgress;

      switch (job_data.type)
      {
      case JobType::Walk:
        registry.emplace_or_replace<ActionWalk>(entity, current_job.entity);
        break;
      // case JobType::Pickup:
      //   registry.emplace<ActionPickup>(entity, &current_job);
      //   break;
      // case JobType::Wear:
      //   registry.emplace<ActionWear>(entity, &current_job);
      //   break;
      // case JobType::Wield:
      //   registry.emplace<ActionWield>(entity, &current_job);
      //   break;
      // case JobType::Drop:
      //   registry.emplace<ActionDrop>(entity, &current_job);
      //   break;
      default:
        // m_create_or_assign_job_progress(current_job, registry);
        break;
      }
    }
    else if (job_data.status == JobStatus2::InProgress)
    {
      switch (job_data.type)
      {
      case JobType::Walk:
        check_component<ActionWalk>(registry, entity, current_job.entity);
        break;
      // case JobType::Pickup:
      //   check_component<ActionPickup>(registry, entity, &current_job);
      //   break;
      // case JobType::Wear:
      //   check_component<ActionWear>(registry, entity, &current_job);
      //   break;
      // case JobType::Wield:
      //   check_component<ActionWield>(registry, entity, &current_job);
      //   break;
      // case JobType::Drop:
      //   check_component<ActionDrop>(registry, entity, &current_job);
      //   break;
      // case JobType::Harvest:
      // case JobType::Break:
      // case JobType::Dig:
      // case JobType::PrepareFirecamp:
      // case JobType::StartFire:
      //   m_update_tile_job(current_job, delta, entity, registry);
      //   break;
      default:
        break;
      }
    }
    if (job_data.status == JobStatus2::Finished)
    {
      agent.jobs.pop();
    }
  }
}

void JobSystem::m_update_tile_job(const Job& job,
                                  const double delta,
                                  const entt::entity agent,
                                  entt::registry& registry)
{
  if (job.status != JobStatus::InProgress)
  {
    return;
  }

  if (!registry.valid(job.progress_entity))
  {
    job.status = JobStatus::Finished;
    return;
  }

  auto& job_progress = registry.get<JobProgress>(job.progress_entity);

  job_progress.time_left -= delta;

  if (job_progress.time_left < 0.0)
  {
    job.status = JobStatus::Finished;
    registry.destroy(job.progress_entity);

    const auto& tile = m_world.get(job.target.position.x, job.target.position.y, job.target.position.z);

    // Check if target tile is still there
    if (tile.id != job.target.id)
    {
      return;
    }

    const auto& tile_data = m_world.get_tile_data(tile.id);
    const auto& action = tile_data.actions.at(job.type);
    const auto& target_position = job.target.position;

    m_world.replace(tile.id, action.turns_into, target_position.x, target_position.y, target_position.z);

    if (action.gives.empty())
    {
      return;
    }

    const auto& position = registry.get<Position>(agent);

    for (const auto& item : action.gives)
    {
      const auto drop = registry.create();

      if (action.gives_in_place)
      {
        registry.emplace<Position>(drop,
                                   static_cast<double>(target_position.x),
                                   static_cast<double>(target_position.y),
                                   static_cast<double>(target_position.z));
      }
      else
      {
        registry.emplace<Position>(drop, position.x, position.y, position.z);
      }

      registry.emplace<Sprite>(drop,
                               Sprite{
                                   .resource_id = m_world.get_texture_id(),
                                   .id = item.first,
                                   .category = frame_data_type::item,
                                   .layer_z = renderer::layer_z_offset_items,
                               });
      registry.emplace<Item>(drop, item.first);
    }
  }
}

void JobSystem::m_create_or_assign_job_progress(const Job& job, entt::registry& registry)
{
  const auto& target_position = job.target.position;
  const auto existing_entity = m_world.spatial_hash.get_by_component<JobProgress>(
      target_position.x, target_position.y, target_position.z, registry);

  if (registry.valid(existing_entity))
  {
    const auto& job_progress = registry.get<JobProgress>(existing_entity);
    if (job_progress.type == job.type)
    {
      job.progress_entity = existing_entity;
    }
  }
  else
  {
    const auto entity = registry.create();
    registry.emplace<JobProgress>(entity, job.type, 0.1);
    registry.emplace<Position>(entity,
                               static_cast<double>(target_position.x),
                               static_cast<double>(target_position.y),
                               static_cast<double>(target_position.z));

    job.progress_entity = entity;
  }
}

}  // namespace dl
