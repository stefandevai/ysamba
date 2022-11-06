#include "./world.hpp"

namespace dl
{
  World::World(Tilemap& tilemap)
    : m_tilemap(tilemap)
  {

  }

  World::~World() {}

  void World::add(Body* body)
  {
    m_bodies.push_back(body);
  }

  void World::update(const uint32_t delta)
  {
    for (auto& body : m_bodies)
    {
      if (!body->is_static && (body->velocity.x != 0 || body->velocity.y != 0))
      {
        m_move(body);
      }
    }
  }

  void World::m_move(Body* body)
  {
    const int x_candidate = body->position.x + body->velocity.x;
    const int y_candidate = body->position.y + body->velocity.y;

    const auto& target_tile = m_tilemap.get(x_candidate, y_candidate, body->position.z);

    if (target_tile.walkable)
    {
      body->position.x += body->velocity.x;
      body->position.y += body->velocity.y;
    }

    body->velocity.x = 0;
    body->velocity.y = 0;
  }
}
