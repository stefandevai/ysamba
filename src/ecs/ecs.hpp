#pragma once

#include <entt/entt.hpp>
#include <libtcod.hpp>
#include "systems/movement.hpp"
#include "systems/render.hpp"

namespace dl
{
  class ECS
  {
    public:
      ECS();
      void load();
      void update(const uint32_t delta);
      void render(TCOD_Console& console);

    private:
      entt::registry m_registry;
      MovementSystem m_movement_system;
      RenderSystem m_render_system;
  };
}

