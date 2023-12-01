#pragma once

#include <entt/entity/registry.hpp>

namespace dl
{
class World;

class WalkSystem
{
 public:
  WalkSystem();

  void update(entt::registry& registry);
};
}  // namespace dl
