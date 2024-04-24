#pragma once

#include <entt/entity/registry.hpp>

#include "ai/actions/utils.hpp"
#include "ai/actions/walk.hpp"
#include "config.hpp"
#include "core/maths/vector.hpp"
#include "ecs/components/job_data.hpp"
#include "ecs/components/society_agent.hpp"
#include "world/society/job.hpp"
#include "world/target.hpp"
#include "world/world.hpp"

namespace dl::action::generic_tile
{
struct CreateJobParams
{
  World& world;
  entt::registry& registry;
  const Vector3i& position;
  JobType job_type{};
  const std::vector<entt::entity> entities;
};

static bool job(CreateJobParams params)
{
  if (!utils::validate_tile_action({.registry = params.registry,
                                    .world = params.world,
                                    .position = params.position,
                                    .job_type = params.job_type}))
  {
    // TODO: Notify player that validation failed for action
    return false;
  }

  const auto& tile = params.world.get(params.position);
  const auto& qualities_required = tile.actions.at(params.job_type).qualities_required;
  bool job_assigned = false;

  const auto assign_job = [&params, &tile, &qualities_required, &job_assigned](const entt::entity entity)
  {
    // Check if the agent has the necessary qualities to perform the action
    if (!qualities_required.empty())
    {
      if (!utils::has_qualities_required({.world = params.world,
                                          .registry = params.registry,
                                          .entity = entity,
                                          .qualities_required = qualities_required}))
      {
        // TODO: Notify player that items with required qualities are needed
        return;
      }
    }

    auto& agent = params.registry.get<SocietyAgent>(entity);
    const auto job = params.registry.create();

    params.registry.emplace<Target>(job, static_cast<Vector3>(params.position), tile.id);
    params.registry.emplace<JobData>(job, params.job_type);

    // TODO: Create walk job if needed on the respective action system
    walk::job({
        .registry = params.registry,
        .agent_entity = entity,
        .agent = &agent,
        .position = params.position,
        .offset = 1,
    });

    agent.push_job(Job{config::ai::default_job_priority, job});
    job_assigned = true;
  };

  std::for_each(params.entities.begin(), params.entities.end(), assign_job);

  return job_assigned;
}
}  // namespace dl::action::generic_tile
