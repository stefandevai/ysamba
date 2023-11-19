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

      // Check if the tile changed
      if (std::abs(position.x - x_candidate) >= 1.0 || std::abs(position.y - y_candidate) >= 1.0)
      {
        const auto x_round = std::round(x_candidate);
        const auto y_round = std::round(y_candidate);
        const auto& target_tile = m_world.get(x_round, y_round, z_candidate);
        auto can_walk = true;

        if (!target_tile.flags.contains("WALKABLE"))
        {
          can_walk = false;
        }

        if (m_collides(registry, x_round, y_round))
        {
          can_walk = false;
        }

        if (can_walk)
        {
          registry.patch<Position>(entity, [x_candidate, y_candidate](auto& position) {
            position.x = x_candidate;
            position.y = y_candidate;
          });
        }
      }
      // Tile has not changed
      else
      {
        position.x = x_candidate;
        position.y = y_candidate;
      }

      velocity.x = 0.;
      velocity.y = 0.;
    });
  }

  bool PhysicsSystem::m_collides(entt::registry &registry, const int x, const int y)
  {
    const auto& entities = m_world.spatial_hash.get(x, y);

    for (const auto entity : entities)
    {
      if (registry.all_of<Position>(entity))
      {
        auto& position = registry.get<Position>(entity);

        if (std::round(position.x) == x && std::round(position.y) == y)
        {
          return true;
        }
      }
    }

    return false;
  }
}
