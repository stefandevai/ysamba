#include "./physics.hpp"

#include <spdlog/spdlog.h>

#include <entt/entity/registry.hpp>
#include <libtcod.hpp>

#include "core/maths/vector.hpp"
#include "ecs/components/biology.hpp"
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

    Vector3 candidate_position{
        position.x + velocity.x * position_variation,
        position.y + velocity.y * position_variation,
        position.z,
    };

    Vector2i absolute_advance{
        std::abs(std::round(candidate_position.x) - std::round(position.x)),
        std::abs(std::round(candidate_position.y) - std::round(position.y)),
    };

    auto target_position = Position{position};

    // Test collision for the tiles we want advance
    if (absolute_advance.x > 0 || absolute_advance.y > 0)
    {
      bool distant_collide = false;
      auto last_position_x = position.x;
      auto last_position_y = position.y;

      TCOD_bresenham_data_t bresenham_data;
      int x_round = std::round(position.x);
      int y_round = std::round(position.y);

      TCOD_line_init_mt(x_round,
                        y_round,
                        x_round + absolute_advance.x * velocity.x,
                        y_round + absolute_advance.y * velocity.y,
                        &bresenham_data);

      // While loop instead of do while to avoid checking the current position
      while (!TCOD_line_step_mt(&x_round, &y_round, &bresenham_data))
      {
        distant_collide = m_collides(registry, entity, x_round, y_round, candidate_position.z);

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
        target_position.x = candidate_position.x;
        target_position.y = candidate_position.y;
      }
      // Otherwise, advance to the last walkable position
      // if it's not the current position.
      else
      {
        const auto walkable_advance_x = std::abs(last_position_x - position.x);
        const auto walkable_advance_y = std::abs(last_position_y - position.y);

        if (walkable_advance_x > 0 || walkable_advance_y > 0)
        {
          target_position.x = position.x + walkable_advance_x * velocity.x;
          target_position.y = position.y + walkable_advance_y * velocity.y;
        }
        // Check if current tile is a slope
        else
        {
          const auto& tiles = m_world.get_all(std::round(position.x), std::round(position.y), std::round(position.z));

          if (tiles.terrain.flags.contains("SLOPE"))
          {
            target_position = m_climb_slope(position,
                                            Position{candidate_position.x, candidate_position.y, position.z + 1},
                                            tiles.terrain.climbs_to);
          }
        }
      }
    }
    // If there was a fractional advance without tile change, just update our position
    else
    {
      target_position.x = candidate_position.x;
      target_position.y = candidate_position.y;
    }

    // Update the position if it's different from the last position
    if (target_position.x != position.x || target_position.y != position.y || target_position.z != position.z)
    {
      registry.patch<Position>(entity, [&target_position](auto& position) { position = target_position; });
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

Position PhysicsSystem::m_climb_slope(const Position& position,
                                      const Position& candidate_position,
                                      const Direction climbs_to)
{
  Position rounded_position{std::round(position.x), std::round(position.y), std::round(position.z)};
  Vector2i advance{
      std::round(candidate_position.x) - rounded_position.x,
      std::round(candidate_position.y) - rounded_position.y,
  };

  if (climbs_to == Direction::Top && advance.y < 0 && advance.x == 0)
  {
    return Position{
        rounded_position.x,
        rounded_position.y - 1,
        rounded_position.z + 1,
    };
  }
  else if (climbs_to == Direction::Right && advance.y == 0 && advance.x > 0)
  {
    return Position{
        rounded_position.x + 1,
        rounded_position.y,
        rounded_position.z + 1,
    };
  }
  else if (climbs_to == Direction::Bottom && advance.y > 0 && advance.x == 0)
  {
    return Position{
        rounded_position.x,
        rounded_position.y + 1,
        rounded_position.z + 1,
    };
  }
  else if (climbs_to == Direction::Left && advance.y == 0 && advance.x < 0)
  {
    return Position{
        rounded_position.x - 1,
        rounded_position.y,
        rounded_position.z + 1,
    };
  }
  else if (climbs_to == Direction::TopLeft && advance.y < 0 && advance.x < 0)
  {
    return Position{
        rounded_position.x - 1,
        rounded_position.y - 1,
        rounded_position.z + 1,
    };
  }
  else if (climbs_to == Direction::TopRight && advance.y < 0 && advance.x > 0)
  {
    return Position{
        rounded_position.x + 1,
        rounded_position.y - 1,
        rounded_position.z + 1,
    };
  }
  else if (climbs_to == Direction::BottomRight && advance.y > 0 && advance.x > 0)
  {
    return Position{
        rounded_position.x + 1,
        rounded_position.y + 1,
        rounded_position.z + 1,
    };
  }
  else if (climbs_to == Direction::BottomLeft && advance.y > 0 && advance.x < 0)
  {
    return Position{
        rounded_position.x - 1,
        rounded_position.y + 1,
        rounded_position.z + 1,
    };
  }

  return position;
}
}  // namespace dl
