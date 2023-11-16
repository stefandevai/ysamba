#pragma once

#include <entt/entity/registry.hpp>
#include <libtcod.hpp>
#include "systems/movement.hpp"
#include "systems/render.hpp"

namespace dl
{
  class Camera;

  class ECS
  {
    public:
      ECS(entt::registry& registry, const Camera& camera);
      void load();
      void update(const uint32_t delta);
      void render(TCOD_Console& console);

    private:
      entt::registry& m_registry;
      const Camera& m_camera;
      MovementSystem m_movement_system;
      RenderSystem m_render_system;
  };
}

