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

namespace dl::action::generic_item
{
struct CreateJobParams
{
  entt::registry& registry;
  const Vector3i& position;
  JobType job_type{};
  const std::vector<entt::entity>& entities;
  entt::entity item = entt::null;
};

static bool job(CreateJobParams params)
{
  assert(params.item != entt::null && "No item provided for job");

  bool job_assigned = false;

  const auto assign_job = [&params, &job_assigned](const entt::entity entity)
  {
    const auto job = params.registry.create();
    params.registry.emplace<Target>(job, static_cast<Vector3>(params.position), static_cast<uint32_t>(params.item));
    params.registry.emplace<JobData>(job, params.job_type);

    action::walk::job({
        .registry = params.registry,
        .agent_entity = entity,
        .position = params.position,
        .offset = 1,
    });

    auto& agent = params.registry.get<SocietyAgent>(entity);
    agent.push_job(Job{config::ai::default_job_priority, job});

    job_assigned = true;
  };

  std::for_each(params.entities.begin(), params.entities.end(), assign_job);

  return job_assigned;
}
}  // namespace dl::action::generic_item
