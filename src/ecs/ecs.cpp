#include "./ecs.hpp"

#include <spdlog/spdlog.h>

#include <entt/entity/registry.hpp>

#include "graphics/camera.hpp"
#include "graphics/renderer.hpp"

namespace dl
{
ECS::ECS(entt::registry& registry, World& world, Camera& camera)
    : m_registry(registry), m_world(world), m_camera(camera)
{
}

void ECS::load() {}

void ECS::update(const double delta)
{
  if (m_turn_delay <= 0)
  {
    m_turn_delay = 0.5;
  }
  else
  {
    m_turn_delay -= delta;
    return;
  }
  m_game_system.update();
  m_physics_system.update(m_registry, delta);
  m_society_system.update(m_registry, delta);
  m_harvest_system.update(m_registry, delta);
  m_inspector_system.update(m_registry, m_camera);
}

void ECS::render(Renderer& renderer) { m_render_system.update(m_registry, renderer, m_camera); }
}  // namespace dl
