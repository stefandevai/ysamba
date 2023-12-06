#pragma once

#include <cstdint>

#include "core/maths/vector.hpp"

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
  uint32_t id;
  int priority = 0;
  Vector3i target;
  mutable JobStatus status = JobStatus::Waiting;
  int insertion_index = 0;
  static int current_index;

  Job(const uint32_t id, const int priority, const Vector3i& target) : id(id), priority(priority), target(target)
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
