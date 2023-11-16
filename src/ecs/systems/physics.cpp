#include "./physics.hpp"
#include "../../world/world.hpp"
#include "../components/position.hpp"
#include "../components/velocity.hpp"

namespace dl
{
  PhysicsSystem::PhysicsSystem(World& world)
    : m_world(world)
  {

  }

  void PhysicsSystem::update(entt::registry &registry, const uint32_t delta)
  {
    auto view = registry.view<Position, const Velocity>();
    view.each([this, delta](auto &position, const auto &velocity) {
      if (velocity.x == 0 && velocity.y == 0)
      {
        return;
      }

      const auto delta_s = delta / 1000.0;
      const double x_candidate = position.x + velocity.x * delta_s;
      const double y_candidate = position.y + velocity.y * delta_s;

      const auto& target_tile = m_world.get(x_candidate, y_candidate, position.z);

      if (target_tile.walkable)
      {
        position.x = x_candidate;
        position.y = y_candidate;
      }
    });
  }
}
