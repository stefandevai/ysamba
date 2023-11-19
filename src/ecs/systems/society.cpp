#include "./society.hpp"

#include <spdlog/spdlog.h>

#include <entt/entity/registry.hpp>

#include "../../world/world.hpp"
#include "../components/biology.hpp"
#include "../components/society_agent.hpp"
#include "../components/velocity.hpp"

namespace dl
{
SocietySystem::SocietySystem() {}

void SocietySystem::update(entt::registry& registry, const uint32_t delta)
{
  auto view = registry.view<SocietyAgent, Biology>();
  view.each([this, &registry, delta](auto entity, auto& agent, auto& biology) {
    if (agent.state == SocietyAgent::State::Idle)
    {
      if (agent.time_to_next_action <= 0)
      {
        agent.time_to_next_action = 2000;
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
    if (agent.state == SocietyAgent::State::Walking)
    {
      if (agent.time_to_next_action <= 0)
      {
        agent.time_to_next_action = 2000;
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
        velocity_x = -biology.current_speed;
      }
      else if (x_dir < .66f)
      {
        velocity_x = biology.current_speed;
      }

      if (y_dir < .33f)
      {
        velocity_y = -biology.current_speed;
      }
      else if (y_dir < .66f)
      {
        velocity_y = biology.current_speed;
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
