#pragma once

#include <entt/entt.hpp>
#include <libtcod.hpp>

namespace dl
{
  class Camera;
  class Renderer;

  class RenderSystem
  {
    public:
      RenderSystem();

      void update(entt::registry &registry, Renderer& renderer, const Camera& camera);
  };
}
