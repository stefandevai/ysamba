#pragma once

#include <stack>

namespace dl
{
struct WalkPath
{
  std::stack<std::pair<int, int>> steps{};
};
}  // namespace dl
