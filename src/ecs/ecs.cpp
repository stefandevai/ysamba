#include "./ecs.hpp"
#include "./components/velocity.hpp"
#include "../world/camera.hpp"

namespace dl
{
  ECS::ECS(entt::registry& registry, const Camera& camera) : m_registry(registry), m_camera(camera) { }

  void ECS::load()
  {
  }

  void ECS::update(const uint32_t delta)
  {
    m_movement_system.update(m_registry, delta);
  }

  void ECS::render(TCOD_Console& console)
  {
    m_render_system.update(m_registry, console, m_camera);
  }
}


