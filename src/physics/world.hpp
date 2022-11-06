#pragma once

#include <vector>
#include "../player/player.hpp"
#include "../tilemap/tilemap.hpp"
#include "./body.hpp"

namespace dl
{
  class World
  {
    public:
      World(Tilemap& tilemap);
      ~World();

      void add(Body* body);
      void update(const uint32_t delta);

    private:
      Tilemap& m_tilemap;
      std::vector<Body*> m_bodies;

      void m_move(Body* body);
  };
}
