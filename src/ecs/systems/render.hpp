#pragma once

#include <entt/entt.hpp>
#include <libtcod.hpp>

namespace dl
{
  class World;
  class Camera;
  class Renderer;

  class RenderSystem
  {
    public:
      RenderSystem(World& world);
      void update(entt::registry &registry, Renderer& renderer, const Camera& camera);

    private:
      World& m_world;
  };
}
