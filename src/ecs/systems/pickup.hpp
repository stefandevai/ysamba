#pragma once

#include <entt/entity/registry.hpp>

namespace dl
{
class World;

class PickupSystem
{
 public:
  PickupSystem(World& world);

  void update(entt::registry& registry, const double delta);

 private:
  World& m_world;
};
}  // namespace dl
