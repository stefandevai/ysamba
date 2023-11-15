#include "./ecs.hpp"
#include "./components/position.hpp"
#include "./components/velocity.hpp"
#include "./components/visibility.hpp"

namespace dl
{
  ECS::ECS() { }

  void ECS::load()
  {
    const auto entity1 = m_registry.create();
    m_registry.emplace<Position>(entity1, 0., 0., 0.);
    m_registry.emplace<Velocity>(entity1, 0.01, 0., 0.);
    m_registry.emplace<Visibility>(entity1, 'P', 255, 255, 255);

    const auto entity2 = m_registry.create();
    m_registry.emplace<Position>(entity2, 3., 3., 0.);
    m_registry.emplace<Velocity>(entity2, 0.01, 0., 0.);
    m_registry.emplace<Visibility>(entity2, 'P', 255, 255, 255);
  }

  void ECS::update()
  {
    m_movement_system.update(m_registry);
  }

  void ECS::render(TCOD_Console& console)
  {
    m_render_system.update(m_registry, console);
  }
}


