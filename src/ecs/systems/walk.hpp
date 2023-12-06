#pragma once

#include <entt/entity/registry.hpp>

namespace dl
{
class World;

class WalkSystem
{
 public:
  WalkSystem(World& world);

  void update(entt::registry& registry);

 private:
  World& m_world;
};
}  // namespace dl
