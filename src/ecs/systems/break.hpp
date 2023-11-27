#pragma once

#include <entt/entity/fwd.hpp>

namespace dl
{
class World;

class BreakSystem
{
 public:
  BreakSystem(World& world);
  void update(entt::registry& registry, const double delta);

 private:
  World& m_world;
};
}  // namespace dl
