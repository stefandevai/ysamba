#pragma once

#include <entt/entity/registry.hpp>
#include <stack>

namespace dl
{
struct ItemTarget
{
  ItemTarget(entt::entity entity, int x, int y, int z) : entity(entity), x(x), y(y), z(z) {}
  ItemTarget() : x(0), y(0), z(0) {}

  entt::entity entity = entt::null;
  int x, y, z;
  std::stack<std::pair<int, int>> path{};
};
}  // namespace dl
