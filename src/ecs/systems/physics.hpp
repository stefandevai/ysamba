#pragma once

#include <entt/entity/fwd.hpp>

#include "ecs/components/position.hpp"

namespace dl
{
class World;

class PhysicsSystem
{
 public:
  PhysicsSystem(World& world);

  void update(entt::registry& registry);

 private:
  World& m_world;

  bool m_collides(const int x, const int y, const int z, entt::registry& registry);
  Position m_get_climb_position(const Position& positon, const Position& candidate_position);
};
}  // namespace dl
