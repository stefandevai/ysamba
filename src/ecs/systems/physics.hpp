#pragma once

#include <entt/entt.hpp>
#include "../../world/spatial_hash.hpp"

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
      SpatialHash m_spatial_hash{100, 100, 10};
  };
}
