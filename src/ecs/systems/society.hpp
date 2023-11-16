#pragma once

#include <entt/entt.hpp>

namespace dl
{
  class SocietySystem
  {
    public:
      SocietySystem();

      void update(entt::registry &registry, const uint32_t delta);
  };
}

