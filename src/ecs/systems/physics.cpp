#include "./physics.hpp"

#include <spdlog/spdlog.h>

#include <entt/entity/registry.hpp>
#include <libtcod.hpp>

#include "ecs/components/biology.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/velocity.hpp"
#include "world/world.hpp"

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
    const double z_candidate = position.z;
    double x_candidate = position.x + velocity.x * (biology.speed / speed_divide_factor);
    double y_candidate = position.y + velocity.y * (biology.speed / speed_divide_factor);

    size_t advance_x = std::abs(x_candidate - position.x);
    size_t advance_y = std::abs(y_candidate - position.y);

    auto target_x = position.x;
    auto target_y = position.y;

    if (advance_x > 0 || advance_y > 0)
    {
      bool distant_collide = false;
      auto last_position_x = position.x;
      auto last_position_y = position.y;

      TCOD_bresenham_data_t bresenham_data;
      int x_round = std::round(position.x);
      int y_round = std::round(position.y);

      TCOD_line_init_mt(
          x_round, y_round, x_round + advance_x * velocity.x, y_round + advance_y * velocity.y, &bresenham_data);

      // While loop instead of do while to avoid checking the current position
      while (!TCOD_line_step_mt(&x_round, &y_round, &bresenham_data))
      {
        distant_collide = m_collides(registry, entity, x_round, y_round, z_candidate);

        if (distant_collide)
        {
          break;
        }
        last_position_x = x_round;
        last_position_y = y_round;
      }

      if (!distant_collide)
      {
        target_x = x_candidate;
        target_y = y_candidate;
      }
      else
      {
        advance_x = std::abs(last_position_x - position.x);
        advance_y = std::abs(last_position_y - position.y);

        if (advance_x > 0 || advance_y > 0)
        {
          target_x = position.x + advance_x * velocity.x;
          target_y = position.y + advance_y * velocity.y;
        }
      }
    }
    else
    {
      target_x = x_candidate;
      target_y = y_candidate;
    }

    // Update the position if it's different from the last position
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
  spdlog::info("Collides? ({}, {})", x, y);
  auto& target_tile = m_world.get(x, y, z);

  if (!target_tile.flags.contains("WALKABLE"))
  {
    spdlog::critical("Yes! ({}, {})", x, y);
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
        spdlog::critical("Yes! ({}, {}), person position: ({}, {})", x, y, position.x, position.y);
        return true;
      }
    }
  }

  spdlog::debug("No! ({}, {})", x, y);
  return false;
}
}  // namespace dl
