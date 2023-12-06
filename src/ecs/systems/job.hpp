#pragma once

#include <entt/entity/registry.hpp>

namespace dl
{
class World;

class JobSystem
{
 public:
  JobSystem(World& world);

  void update(entt::registry& registry);

 private:
  World& m_world;
};
}  // namespace dl
