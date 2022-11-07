#include "./physics_layer.hpp"

namespace dl
{
  PhysicsLayer::PhysicsLayer(World& world)
    : m_world(world)
  {

  }

  void PhysicsLayer::add(Body* body)
  {
    m_bodies.push_back(body);
  }

  void PhysicsLayer::update(const uint32_t delta)
  {
    for (auto& body : m_bodies)
    {
      if (!body->is_static && (body->velocity.x != 0 || body->velocity.y != 0))
      {
        m_move(body);
      }
    }
  }

  void PhysicsLayer::m_move(Body* body)
  {
    const int x_candidate = body->position.x + body->velocity.x;
    const int y_candidate = body->position.y + body->velocity.y;

    const auto& target_tile = m_world.get(x_candidate, y_candidate, body->position.z);

    if (target_tile.walkable)
    {
      body->position.x += body->velocity.x;
      body->position.y += body->velocity.y;
    }

    body->velocity.x = 0;
    body->velocity.y = 0;
  }
}
