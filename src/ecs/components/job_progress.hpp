#pragma once

#include <entt/entity/entity.hpp>

#include "world/society/job_type.hpp"

namespace dl
{
struct JobProgress
{
  JobType type;
  uint32_t total_cost;
  uint32_t progress = 0;
  std::vector<entt::entity> agents{};

  JobProgress() = default;
  JobProgress(const JobType type, const uint32_t cost) : type(type), total_cost(cost) {}
};

template <typename Archive>
void serialize(Archive& archive, JobProgress& job_progress)
{
  archive(job_progress.type, job_progress.total_cost, job_progress.progress, job_progress.agents);
}
}  // namespace dl
