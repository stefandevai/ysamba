#pragma once

#include <entt/entity/entity.hpp>

namespace dl
{
struct Job2
{
  int priority = 0;
  int insertion_index = 0;
  static int current_index;
  entt::entity entity = entt::null;

  Job2() = default;

  Job2(const int priority, const entt::entity entity) : priority(priority), entity(entity)
  {
    insertion_index = ++current_index;
  }

  bool operator<(const Job2& rhs) const
  {
    if (priority == rhs.priority)
    {
      return insertion_index > rhs.insertion_index;
    }
    return priority > rhs.priority;
  }
};

template <typename Archive>
void serialize(Archive& archive, Job2& job)
{
  archive(job.priority, job.insertion_index, job.entity, job.current_index);
}
}  // namespace dl
