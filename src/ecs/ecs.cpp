#include "./ecs.hpp"

#include <spdlog/spdlog.h>

#include "../graphics/camera.hpp"
#include "../graphics/renderer.hpp"
#include "./components/velocity.hpp"

namespace dl
{
ECS::ECS(entt::registry& registry, World& world, Camera& camera)
    : m_registry(registry), m_world(world), m_camera(camera)
{
}

void ECS::load() {}

void ECS::update(const uint32_t delta)
{
  if (m_turn_delay <= 0)
  {
    m_turn_delay = 200;
  }
  else
  {
    m_turn_delay -= delta;
    return;
  }
  m_game_system.update();
  m_physics_system.update(m_registry, delta);
  m_society_system.update(m_registry, delta);
}

void ECS::render(Renderer& renderer) { m_render_system.update(m_registry, renderer, m_camera); }
}  // namespace dl
