#pragma once

#include <entt/entity/entity.hpp>

#include "world/society/job_type.hpp"

namespace dl
{
enum class JobStatus2
{
  Waiting,
  InProgress,
  Finished,
};

struct JobData
{
  JobType type;
  JobStatus2 status = JobStatus2::Waiting;
  entt::entity progress_entity = entt::null;
};

template <typename Archive>
void serialize(Archive& archive, JobData& data)
{
  archive(data.type, data.status);
}
}  // namespace dl
