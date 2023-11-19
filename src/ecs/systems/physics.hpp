#pragma once

#include <entt/entt.hpp>

namespace dl
{
  class World;

  class PhysicsSystem
  {
    public:
      PhysicsSystem(World& world);

      void update(entt::registry &registry, const uint32_t delta);

    private:
      World& m_world;

      bool m_collides(entt::registry &registry, const int x, const int y);
  };
}
