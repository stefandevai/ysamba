#include "./society.hpp"

#include <spdlog/spdlog.h>

#include <entt/entity/registry.hpp>

#include "../../core/random.hpp"
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
SocietySystem::SocietySystem() {}

void SocietySystem::update(entt::registry& registry, const double delta)
{
  auto view = registry.view<SocietyAgent>();
  view.each([&registry, delta](auto entity, auto& agent) {
    if (agent.state == SocietyAgent::State::Harvesting)
    {
      if (!registry.all_of<ActionHarvest>(entity))
      {
        registry.emplace<ActionHarvest>(entity);
      }
    }
    else if (agent.state == SocietyAgent::State::Idle)
    {
      if (agent.time_to_next_action <= 0)
      {
        agent.time_to_next_action = 1.0;
        const auto n = random::get_real();

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
        const auto n = random::get_real();
        if (n < .4f)
        {
          agent.state = SocietyAgent::State::Idle;
        }
      }
      else
      {
        agent.time_to_next_action -= delta;
      }

      const auto x_dir = random::get_real();
      const auto y_dir = random::get_real();

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
