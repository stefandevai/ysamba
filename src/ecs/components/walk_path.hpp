#pragma once

#include <stack>

namespace dl
{
struct WalkPath
{
  std::stack<std::pair<int, int>> steps{};
};

template <typename Archive>
void serialize(Archive& archive, WalkPath& path)
{
  archive(path.steps);
}
}  // namespace dl
