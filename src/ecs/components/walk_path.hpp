#pragma once

#include <vector>

#include "core/maths/vector.hpp"

namespace dl
{
struct WalkPath
{
  std::vector<Vector3i> steps{};
};

template <typename Archive>
void serialize(Archive& archive, WalkPath& path)
{
  archive(path.steps);
}
}  // namespace dl
