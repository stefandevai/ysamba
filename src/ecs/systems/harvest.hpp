#pragma once

#include <entt/entity/fwd.hpp>

namespace dl
{
class World;

class HarvestSystem
{
 public:
  HarvestSystem(World& world);
  void update(entt::registry& registry, const double delta);

 private:
  World& m_world;
};
}  // namespace dl
