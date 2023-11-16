#pragma once

#include <entt/entity/registry.hpp>
#include <libtcod.hpp>
#include "systems/physics.hpp"
#include "systems/render.hpp"
#include "systems/society.hpp"

namespace dl
{
  class Camera;
  class World;

  class ECS
  {
    public:
      ECS(entt::registry& registry, World& world, const Camera& camera);
      void load();
      void update(const uint32_t delta);
      void render(TCOD_Console& console);

    private:
      entt::registry& m_registry;
      World& m_world;
      const Camera& m_camera;
      PhysicsSystem m_physics_system{m_world};
      RenderSystem m_render_system;
      SocietySystem m_society_system;
  };
}

