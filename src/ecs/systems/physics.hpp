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

  void update(entt::registry& registry, const double delta);

 private:
  World& m_world;

  bool m_collides(entt::registry& registry, entt::entity, const int x, const int y, const int z);
  Position m_get_climb_position(const Position& positon, const Position& candidate_position);
};
}  // namespace dl
