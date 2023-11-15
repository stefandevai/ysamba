#pragma once

#include <entt/entt.hpp>

namespace dl
{
  class MovementSystem
  {
    public:
      MovementSystem();

      void update(entt::registry &registry, const uint32_t delta);
  };
}
