#pragma once

#include <cstdint>
#include <entt/entity/registry.hpp>

#include "./job_type.hpp"
#include "core/maths/vector.hpp"
#include "world/target.hpp"

namespace dl
{
enum class JobStatus
{
  Waiting,
  InProgress,
  Finished,
};

class Job
{
 public:
  JobType type;
  int priority = 0;
  Target target;
  mutable double time_left = 0.05;
  mutable entt::entity progress_entity = entt::null;
  mutable JobStatus status = JobStatus::Waiting;
  int insertion_index = 0;
  static int current_index;

  Job() = default;

  Job(const JobType type, const int priority, const Target& target) : type(type), priority(priority), target(target)
  {
    insertion_index = ++current_index;
  }

  bool operator<(const Job& rhs) const
  {
    if (priority == rhs.priority)
    {
      return insertion_index > rhs.insertion_index;
    }
    return priority > rhs.priority;
  }
};

template <typename Archive>
void serialize(Archive& archive, Job& job)
{
  archive(job.type, job.priority, job.target, job.time_left, job.status, job.insertion_index);
}
}  // namespace dl
