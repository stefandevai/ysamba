#include "./society.hpp"

#include <spdlog/spdlog.h>

#include <entt/entity/registry.hpp>

#include "../../world/world.hpp"
#include "../components/action_harvest.hpp"
#include "../components/biology.hpp"
#include "../components/position.hpp"
#include "../components/rectangle.hpp"
#include "../components/society_agent.hpp"
#include "../components/velocity.hpp"
#include "../components/visibility.hpp"

namespace dl
{
SocietySystem::SocietySystem(World& world) : m_world(world) {}

void SocietySystem::update(entt::registry& registry, const double delta)
{
  auto view = registry.view<SocietyAgent>();
  view.each([this, &registry, delta](auto entity, auto& agent) {
    if (agent.state == SocietyAgent::State::Harvesting)
    {
      // If the entity doesn't have a harvest action, add it
      if (!registry.all_of<ActionHarvest>(entity))
      {
        registry.emplace<ActionHarvest>(entity);
      }

      const auto& position = registry.get<Position>(entity);
      auto& action_harvest = registry.get<ActionHarvest>(entity);

      if (!action_harvest.target)
      {
        action_harvest.target = m_world.search_by_flag("HARVESTABLE", position.x, position.y, position.z);

        if (!action_harvest.target)
        {
          registry.remove<ActionHarvest>(entity);
          agent.state = SocietyAgent::State::Idle;
          return;
        }

        const auto target_rectangle = registry.create();
        registry.emplace<Position>(
            target_rectangle, action_harvest.target.x, action_harvest.target.y, action_harvest.target.z);
        registry.emplace<Rectangle>(target_rectangle, 32, 32, "#ff000066");
      }

      auto target = action_harvest.target;

      if (!m_world.adjacent(target.id, position.x, position.y, position.z))
      {
        // Move towards the target
        return;
      }

      // Harvest
      action_harvest.time_left -= delta;
      spdlog::info("Harvesting: {}", action_harvest.time_left);

      if (action_harvest.time_left < 0.0)
      {
        spdlog::info("Harvested! {} {} {}", target.x, target.y, target.z);

        // Replace the plant tile with grass
        m_world.set(2, target.x, target.y, target.z);

        // Create yuca roots
        const auto yuca_roots = registry.create();
        registry.emplace<Position>(yuca_roots, target.x, target.y, target.z);
        registry.emplace<Visibility>(yuca_roots, "spritesheet-tileset", 6);

        // Finalize action
        action_harvest.target.id = -1;
        /* registry.remove<ActionHarvest>(entity); */
        /* agent.state = SocietyAgent::State::Walking; */
      }
    }
    else if (agent.state == SocietyAgent::State::Idle)
    {
      if (agent.time_to_next_action <= 0)
      {
        agent.time_to_next_action = 1.0;
        const auto n = m_distribution(m_rng);

        if (n < .7f)
        {
          agent.state = SocietyAgent::State::Walking;
        }
      }
      else
      {
        agent.time_to_next_action -= delta;
      }
    }
    else if (agent.state == SocietyAgent::State::Walking || agent.state == SocietyAgent::State::Idle)
    {
      if (agent.time_to_next_action <= 0)
      {
        agent.time_to_next_action = 1.0;
        const auto n = m_distribution(m_rng);
        if (n < .4f)
        {
          agent.state = SocietyAgent::State::Idle;
        }
      }
      else
      {
        agent.time_to_next_action -= delta;
      }

      const auto x_dir = m_distribution(m_rng);
      const auto y_dir = m_distribution(m_rng);

      auto velocity_x = 0.;
      auto velocity_y = 0.;

      if (x_dir < .33f)
      {
        velocity_x = -1.0;
      }
      else if (x_dir < .66f)
      {
        velocity_x = 1.0;
      }

      if (y_dir < .33f)
      {
        velocity_y = -1.0;
      }
      else if (y_dir < .66f)
      {
        velocity_y = 1.0;
      }

      if (registry.all_of<Velocity>(entity))
      {
        registry.patch<Velocity>(entity, [velocity_x, velocity_y](auto& velocity) {
          velocity.x = velocity_x;
          velocity.y = velocity_y;
        });
      }
      else
      {
        registry.emplace<Velocity>(entity, velocity_x, velocity_y, 0.);
      }
    }
  });
}
}  // namespace dl
