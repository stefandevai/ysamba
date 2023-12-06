#pragma once

#include <cstdint>

#include "core/maths/vector.hpp"

namespace dl
{
enum class JobType
{
  Walk,
  Harvest,
  Break,
  Dig,
  Pickup,
};

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
  Vector3i target;
  mutable JobStatus status = JobStatus::Waiting;
  int insertion_index = 0;
  static int current_index;

  Job(const JobType type, const int priority, const Vector3i& target) : type(type), priority(priority), target(target)
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
}  // namespace dl
