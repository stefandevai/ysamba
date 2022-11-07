#pragma once

#include <libtcod.hpp>
#include "../physics/body.hpp"
#include "../world/camera.hpp"

namespace dl
{
  class Player
  {
    public:
      Body body;

      Player();

      void update(const uint32_t delta);
      void render(TCOD_Console& console, const Camera& camera);
      inline bool should_advance_turn() const { return m_should_advance_turn; }

    private:
      uint32_t m_delta_since_move = 0;
      bool m_should_advance_turn = false;

      void m_load();
      void m_move();
  };
}
