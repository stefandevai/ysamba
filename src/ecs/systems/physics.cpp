#include "./physics.hpp"

#include <spdlog/spdlog.h>
#include "../../world/world.hpp"
#include "../components/position.hpp"
#include "../components/velocity.hpp"

namespace dl
{
  PhysicsSystem::PhysicsSystem(World& world)
    : m_world(world)
  { }

  void PhysicsSystem::update(entt::registry &registry, const uint32_t delta)
  {
    auto view = registry.view<Position, Velocity>();
    view.each([this, &registry, delta](auto entity, auto &position, auto &velocity) {
      if (velocity.x == 0 && velocity.y == 0)
      {
        return;
      }

      const auto delta_s = delta / 1000.0;
      const double x_candidate = position.x + velocity.x * delta_s;
      const double y_candidate = position.y + velocity.y * delta_s;
      const double z_candidate = position.z + velocity.z * delta_s;

      const auto& target_tile = m_world.get(x_candidate, y_candidate, z_candidate);

      if (target_tile.flags.contains("WALKABLE"))
      {
        registry.patch<Position>(entity, [x_candidate, y_candidate](auto& position) {
          position.x = x_candidate;
          position.y = y_candidate;
        });
      }

      velocity.x = 0.;
      velocity.y = 0.;
      velocity.z = 0.;
    });
  }
}
