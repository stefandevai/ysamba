#pragma once

#include <entt/entity/registry.hpp>

#include "config.hpp"
#include "core/maths/vector.hpp"
#include "core/random.hpp"
#include "ecs/components/job_data.hpp"
#include "ecs/components/job_data_build_hut.hpp"
#include "ecs/components/job_progress.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/society_agent.hpp"
#include "world/society/job.hpp"
#include "world/target.hpp"

namespace dl::action::create_hut
{
struct CreateJobParams
{
  entt::registry& registry;
  std::vector<entt::entity> entities;
  const Vector3i& position;
  const uint32_t hut_size{};
  int priority = config::ai::default_job_priority;
};

static bool job(CreateJobParams params)
{
  // Create progress entity
  const uint32_t cost_per_tile = 200;
  const uint32_t total_cost = cost_per_tile * params.hut_size * params.hut_size;
  const auto job_progress_entity = params.registry.create();
  auto& progress = params.registry.emplace<JobProgress>(job_progress_entity, JobType::BuildHut, total_cost);
  params.registry.emplace<Position>(job_progress_entity,
                                    static_cast<double>(params.position.x),
                                    static_cast<double>(params.position.y),
                                    static_cast<double>(params.position.z));

  bool job_assigned = false;

  // Assign a build hut job for each agent
  auto assign_build_hut_job = [&params, &progress, job_progress_entity, &job_assigned](const entt::entity entity)
  {
    const auto offset_x = random::get_integer(0, params.hut_size);
    const auto offset_y = random::get_integer(0, params.hut_size);
    const auto job_target = Vector3i{params.position.x + offset_x, params.position.y + offset_y, params.position.z};

    // Create a walk job to walk until the target
    const auto walk_job = params.registry.create();
    params.registry.emplace<Target>(walk_job, job_target);
    params.registry.emplace<JobData>(walk_job, JobType::Walk);

    // Create the main job
    const auto build_hut_job = params.registry.create();
    params.registry.emplace<JobDataBuildHut>(build_hut_job, params.hut_size);

    // Assign the progress entity to the job
    auto& job_data = params.registry.emplace<JobData>(build_hut_job, JobType::BuildHut);
    job_data.progress_entity = job_progress_entity;

    auto& agent = params.registry.get<SocietyAgent>(entity);
    agent.push_job(Job{params.priority, walk_job});
    agent.push_job(Job{params.priority, build_hut_job});

    progress.agents.push_back(entity);

    job_assigned = true;
  };

  std::for_each(params.entities.begin(), params.entities.end(), assign_build_hut_job);

  return job_assigned;
}
}  // namespace dl::action::create_hut
