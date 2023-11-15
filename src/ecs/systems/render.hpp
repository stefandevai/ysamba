#pragma once

#include <entt/entt.hpp>
#include <libtcod.hpp>

namespace dl
{
  class Camera;

  class RenderSystem
  {
    public:
      RenderSystem();

      void update(entt::registry &registry, TCOD_Console& console, const Camera& camera);
  };
}
