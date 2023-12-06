#pragma once

#include <stack>

namespace dl
{
struct TileTarget
{
  TileTarget(uint32_t id, int x, int y, int z) : id(id), x(x), y(y), z(z) {}
  TileTarget() : id(0), x(0), y(0), z(0) {}

  uint32_t id;
  int x, y, z;
  std::stack<std::pair<int, int>> path{};

  explicit operator bool() { return id != 0; }
};
}  // namespace dl
