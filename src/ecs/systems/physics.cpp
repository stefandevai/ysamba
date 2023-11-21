#include "./physics.hpp"

#include <spdlog/spdlog.h>

#include <entt/entity/registry.hpp>

#include "../../world/world.hpp"
#include "../components/biology.hpp"
#include "../components/position.hpp"
#include "../components/velocity.hpp"

namespace dl
{
PhysicsSystem::PhysicsSystem(World& world) : m_world(world) {}

void PhysicsSystem::update(entt::registry& registry, const double delta)
{
  auto view = registry.view<Biology, Position, Velocity>();
  view.each([this, &registry, delta](auto entity, auto& biology, auto& position, auto& velocity) {
    if (velocity.x == 0 && velocity.y == 0)
    {
      return;
    }

    biology.turn_threshold -= biology.speed * 1000.0 * delta;

    if (biology.turn_threshold > 0.0)
    {
      return;
    }

    biology.turn_threshold = 200.0;

    const auto speed_divide_factor = 100.0;
    const double x_candidate = position.x + velocity.x * (biology.speed / speed_divide_factor);
    const double y_candidate = position.y + velocity.y * (biology.speed / speed_divide_factor);
    const double z_candidate = position.z;

    size_t advance_x = std::abs(x_candidate - position.x);
    size_t advance_y = std::abs(y_candidate - position.y);

    auto target_x = position.x;
    auto target_y = position.y;

    size_t counter_x = 0;
    size_t counter_y = 0;

    if (advance_x > 1 || advance_y > 1)
    {
      // TODO: Check if any collisions happen between the object and the target tile
      // so the object can't skip past another obstacle.
      // Use Bresenham algorithm in tcodlib.
    }

    // If can't advance advance_x or advance_y, check if it's possible to advance
    // to the tiles before that one.
    while (counter_x <= advance_x || counter_y <= advance_y)
    {
      const auto x = position.x + velocity.x * (biology.speed / speed_divide_factor) - counter_x;
      const auto x_round = std::round(x);
      const auto collides_x = m_collides(registry, entity, x_round, std::round(position.y), z_candidate);
      const auto y = position.y + velocity.y * (biology.speed / speed_divide_factor) - counter_y;
      const auto y_round = std::round(y);
      const auto collides_y = m_collides(registry, entity, std::round(position.x), y_round, z_candidate);

      // TODO: Resolve collision. If the tile has a collidable object, walking is not possible,
      // otherwise walking is possible

      // Stop early if the object advances this frame and collides in any axis.
      // This avoids the object moving in one axis only only
      if (advance_x > 0 && advance_y > 0 && (collides_x || collides_y))
      {
        break;
      }
      // Stop early if the object advances partially this frame and collides in any axis.
      // This avoids the object moving in one axis only only
      else if (advance_x <= 0 && advance_y <= 0 && (collides_x || collides_y))
      {
        break;
      }

      if (counter_x <= advance_x)
      {
        if (x_round < 0)
        {
          target_x = 0;
          counter_x = advance_x + 1;
        }
        else if (collides_x)
        {
          ++counter_x;
        }
        else
        {
          target_x = x;
          counter_x = advance_x + 1;
        }
      }

      if (counter_y <= advance_y)
      {
        if (y_round < 0)
        {
          target_y = 0;
          counter_y = advance_y + 1;
        }
        else if (collides_y)
        {
          ++counter_y;
        }
        else
        {
          target_y = y;
          counter_y = advance_y + 1;
        }
      }
    }

    if (target_x != position.x || target_y != position.y)
    {
      registry.patch<Position>(entity, [target_x, target_y](auto& position) {
        position.x = target_x;
        position.y = target_y;
      });
    }

    velocity.x = 0.;
    velocity.y = 0.;
  });
}

bool PhysicsSystem::m_collides(entt::registry& registry, entt::entity entity, const int x, const int y, const int z)
{
  auto& target_tile = m_world.get(x, y, z);

  if (!target_tile.flags.contains("WALKABLE"))
  {
    return true;
  }

  const auto& entities = m_world.spatial_hash.get(x, y);

  for (const auto e : entities)
  {
    if (e == entity)
    {
      continue;
    }

    if (registry.all_of<Position, Biology>(e))
    {
      auto& position = registry.get<Position>(e);

      if (std::round(position.x) == x && std::round(position.y) == y)
      {
        return true;
      }
    }
  }

  return false;
}
}  // namespace dl
