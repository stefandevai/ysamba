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
constexpr int WALK_TARGET_OFFSET = 0;

struct CreateJobParams
{
  entt::registry& registry;
  entt::entity agent_entity = entt::null;
  SocietyAgent* agent = nullptr;
  const Vector3i& position;
  int priority = config::ai::default_job_priority;
  int offset = WALK_TARGET_OFFSET;
};

static void create_job(CreateJobParams params)
{
  if (params.agent == nullptr)
  {
    params.agent = &params.registry.get<SocietyAgent>(params.agent_entity);
  }

  const auto job = params.registry.create();
  params.registry.emplace<Target>(job, params.position, 0, params.offset);
  params.registry.emplace<JobData>(job, JobType::Walk);
  params.agent->jobs.push(Job{params.priority, job});
}
}  // namespace dl::action::walk
