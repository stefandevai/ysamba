#include "./movement.hpp"
#include "../components/position.hpp"
#include "../components/velocity.hpp"

namespace dl
{
  MovementSystem::MovementSystem()
  {

  }

  void MovementSystem::update(entt::registry &registry, const uint32_t delta)
  {
    auto view = registry.view<Position, Velocity>();
    view.each([delta](auto &position, auto &velocity) {
      position.x += velocity.x * delta / 1000.0;
      position.y += velocity.y * delta / 1000.0;
    });
  }
}
