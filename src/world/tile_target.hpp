#pragma once

#include <stack>

namespace dl
{
struct TileTarget
{
  TileTarget(int id, int x, int y, int z) : id(id), x(x), y(y), z(z) {}
  TileTarget() : id(-1), x(0), y(0), z(0) {}

  int id, x, y, z;
  std::stack<std::pair<int, int>> path{};

  explicit operator bool() { return id != -1; }
};
}  // namespace dl
