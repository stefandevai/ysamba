#pragma once

#include <entt/entity/registry.hpp>

#include "config.hpp"
#include "core/maths/vector.hpp"
#include "ecs/components/job_data.hpp"
#include "ecs/components/society_agent.hpp"
#include "world/society/job.hpp"
#include "world/target.hpp"

namespace dl::action::walk
{
struct CreateJobParams
{
  entt::registry& registry;
  entt::entity agent_entity = entt::null;
  SocietyAgent* agent = nullptr;
  const Vector3i& position;
  int priority = config::ai::default_job_priority;
};

static void create_job(CreateJobParams params)
{
  if (params.agent == nullptr)
  {
    params.agent = &params.registry.get<SocietyAgent>(params.agent_entity);
  }

  const auto job = params.registry.create();
  params.registry.emplace<Target>(job, params.position);
  params.registry.emplace<JobData>(job, JobType::Walk);
  params.agent->jobs.push(Job{params.priority, job});
}
}  // namespace dl::action::walk
