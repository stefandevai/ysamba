#include "./movement.hpp"
#include "../components/position.hpp"
#include "../components/velocity.hpp"

namespace dl
{
  MovementSystem::MovementSystem()
  {

  }

  void MovementSystem::update(entt::registry &registry)
  {
    auto view = registry.view<const Position, const Velocity>();
    for(auto [entity, pos, vel]: view.each()) {
    }
    /* view.each([](auto &position, auto &velocity) { */ 

    /* }); */
  }
}
