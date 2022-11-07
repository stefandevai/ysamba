#pragma once

#include <vector>
#include "../player/player.hpp"
#include "../world/world.hpp"
#include "./body.hpp"

namespace dl
{
  class PhysicsLayer
  {
    public:
      PhysicsLayer(World& world);

      void add(Body* body);
      void update(const uint32_t delta);

    private:
      World& m_world;
      std::vector<Body*> m_bodies;

      void m_move(Body* body);
  };
}
