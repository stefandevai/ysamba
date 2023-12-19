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
    const auto position_variation = (biology.speed / speed_divide_factor);

    const double z_candidate = position.z;
    double x_candidate = position.x + velocity.x * position_variation;
    double y_candidate = position.y + velocity.y * position_variation;

    size_t advance_x = std::abs(std::round(x_candidate) - std::round(position.x));
    size_t advance_y = std::abs(std::round(y_candidate) - std::round(position.y));

    auto target_x = position.x;
    auto target_y = position.y;
    auto target_z = position.z;

    // Test collision for the tiles we want advance
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

      // Confirm position if there was no collision
      if (!distant_collide)
      {
        target_x = x_candidate;
        target_y = y_candidate;
      }
      // Otherwise, advance to the last walkable position
      // if it's not the current position.
      else
      {
        const auto walkable_advance_x = std::abs(last_position_x - position.x);
        const auto walkable_advance_y = std::abs(last_position_y - position.y);

        if (walkable_advance_x > 0 || walkable_advance_y > 0)
        {
          target_x = position.x + walkable_advance_x * velocity.x;
          target_y = position.y + walkable_advance_y * velocity.y;
        }
        // Check if current tile is a slope
        else
        {
          const auto& entity_position = registry.get<Position>(entity);
          const auto& tiles = m_world.get_all(std::round(entity_position.x), std::round(entity_position.y), position.z);

          spdlog::debug("HREEEEEE1 {} {} {} {}", tiles.terrain.climbs_to, tiles.terrain.name, advance_y, advance_x);
          spdlog::debug("POS: {} {} {}", entity_position.x, entity_position.y, position.z);

          if (tiles.terrain.flags.contains("SLOPE"))
          {
            spdlog::debug("HREEEEEE2 {} {} {}", tiles.terrain.climbs_to, advance_y, advance_x);
            if (tiles.terrain.climbs_to == 0 && advance_y > 0 && advance_x == 0)
            {
              target_x = std::round(entity_position.x);
              target_y = std::round(entity_position.y);
              target_z += 1;
            }
          }
        }
      }
    }
    // If there was a fractional advance without tile change, just update our position
    else
    {
      target_x = x_candidate;
      target_y = y_candidate;
    }

    // Update the position if it's different from the last position
    if (target_x != position.x || target_y != position.y || target_z != position.z)
    {
      registry.patch<Position>(entity, [target_x, target_y, target_z](auto& position) {
        position.x = target_x;
        position.y = target_y;
        position.z = target_z;
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
