#pragma once

#include "world/society/job_type.hpp"

namespace dl
{
struct JobProgress
{
  JobType type;
  double total_time = 0.0;
  double time_left;

  JobProgress() = default;
  JobProgress(const JobType type, const double total_time) : type(type), total_time(total_time), time_left(total_time)
  {
  }
};

template <typename Archive>
void serialize(Archive& archive, JobProgress& job_progress)
{
  archive(job_progress.type, job_progress.total_time, job_progress.time_left);
}
}  // namespace dl
