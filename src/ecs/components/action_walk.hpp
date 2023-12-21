#pragma once

#include "world/society/job.hpp"

namespace dl
{
struct ActionWalk
{
  const Job* job = nullptr;
};

template <typename Archive>
void serialize(Archive& archive, ActionWalk& action)
{
  if (action.job == nullptr)
  {
    return;
  }

  archive(*action.job);
}
}  // namespace dl
