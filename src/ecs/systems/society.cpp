#include "./society.hpp"

#include <spdlog/spdlog.h>

#include <entt/entity/registry.hpp>

#include "core/maths/vector.hpp"
#include "core/random.hpp"
#include "ecs/components/biology.hpp"
#include "ecs/components/movement.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/society_agent.hpp"
#include "ecs/components/visibility.hpp"
#include "world/world.hpp"

// // DEBUG
// #include "constants.hpp"
// #include "ecs/components/action_walk.hpp"
// // DEBUG

namespace dl
{
SocietySystem::SocietySystem(const World& world) : m_world(world) {}

void SocietySystem::update(entt::registry& registry, const double delta)
{
  (void)m_world;
  (void)registry;
  (void)delta;
  // auto view = registry.view<SocietyAgent, Position>();
  // // for (const auto entity : view)
  // // {
  // //   const auto& agent = registry.get<SocietyAgent>(entity);
  // //   if (registry.all_of<ActionWalk>(entity) || agent.jobs.size() > 0)
  // //   {
  // //     return;
  // //   }
  // // }
  //
  // view.each([&registry, delta, this](auto entity, auto& agent, const auto& position) {
  //   if (registry.all_of<ActionWalk>(entity) || agent.jobs.size() > 0)
  //   {
  //     return;
  //   }
  //
  //   const auto x = random::get_integer(-128, 256);
  //   const auto y = random::get_integer(-128, 256);
  //   const auto elevation = m_world.get_elevation(x, y);
  //
  //   if (m_world.is_walkable(x, y, elevation))
  //   {
  //     agent.jobs.push(Job{JobType::Walk, 0, Target{{x, y, elevation}, 0, 0}});
  //   }
  // });

  // auto view = registry.view<SocietyAgent>();
  // view.each([&registry, delta](auto entity, auto& agent) {
  //   if (agent.state == SocietyAgent::State::Harvesting)
  //   {
  //     /* if (!registry.all_of<ActionHarvest>(entity)) */
  //     /* { */
  //     /*   registry.emplace<ActionHarvest>(entity); */
  //     /* } */
  //   }
  //   else if (agent.state == SocietyAgent::State::Idle)
  //   {
  //     if (agent.time_to_next_action <= 0)
  //     {
  //       agent.time_to_next_action = 1.0;
  //       /* const auto n = random::get_real(); */
  //
  //       /* if (n < .7f) */
  //       /* { */
  //       /*   agent.state = SocietyAgent::State::Walking; */
  //       /* } */
  //     }
  //     else
  //     {
  //       agent.time_to_next_action -= delta;
  //     }
  //   }
  //   else if (agent.state == SocietyAgent::State::Walking || agent.state == SocietyAgent::State::Idle)
  //   {
  //     if (agent.time_to_next_action <= 0)
  //     {
  //       agent.time_to_next_action = 1.0;
  //       const auto n = random::get_real();
  //       if (n < .4f)
  //       {
  //         agent.state = SocietyAgent::State::Idle;
  //       }
  //     }
  //     else
  //     {
  //       agent.time_to_next_action -= delta;
  //     }
  //
  //     const auto x_dir = random::get_real();
  //     const auto y_dir = random::get_real();
  //
  //     auto movement_x = 0.;
  //     auto movement_y = 0.;
  //
  //     if (x_dir < .33f)
  //     {
  //       movement_x = -1.0;
  //     }
  //     else if (x_dir < .66f)
  //     {
  //       movement_x = 1.0;
  //     }
  //
  //     if (y_dir < .33f)
  //     {
  //       movement_y = -1.0;
  //     }
  //     else if (y_dir < .66f)
  //     {
  //       movement_y = 1.0;
  //     }
  //
  //     if (registry.all_of<Movement>(entity))
  //     {
  //       registry.patch<Movement>(entity, [movement_x, movement_y](auto& movement) {
  //         movement.direction.x = movement_x;
  //         movement.direction.y = movement_y;
  //       });
  //     }
  //     else
  //     {
  //       registry.emplace<Movement>(entity, Vector3{movement_x, movement_y, 0.});
  //     }
  //   }
  // });
}
}  // namespace dl
