#include "./society.hpp"
#include "../../world/world.hpp"
#include "../components/society_agent.hpp"
#include "../components/biology.hpp"

namespace dl
{
  SocietySystem::SocietySystem()
  {

  }

  void SocietySystem::update(entt::registry &registry, const uint32_t delta)
  {
    auto view = registry.view<SocietyAgent, Biology>();
    view.each([](auto entity, auto& agent, auto& biology) {
        if (agent.state == SocietyAgent::State::Idle)
        {

        }
        else if (agent.state == SocietyAgent::State::Walking)
        {
          
        }
    });
  }
}

