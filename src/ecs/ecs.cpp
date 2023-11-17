#include "./ecs.hpp"
#include "./components/velocity.hpp"
#include "../graphics/renderer.hpp"

namespace dl
{
  ECS::ECS(entt::registry& registry, World& world, const Camera& camera) : m_registry(registry), m_world(world), m_camera(camera) { }

  void ECS::load()
  {
  }

  void ECS::update(const uint32_t delta)
  {
    m_physics_system.update(m_registry, delta);
    m_society_system.update(m_registry, delta);
  }

  void ECS::render(Renderer& renderer)
  {
    m_render_system.update(m_registry, renderer, m_camera);
  }
}


