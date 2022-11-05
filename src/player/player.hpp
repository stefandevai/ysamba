#pragma once

#include "../core/components/position.hpp"

namespace dl
{
  class Player
  {
    public:
      void update(const uint32_t delta);
      void render(TCOD_Console& console);

    private:
      Position m_position{10,10,0};
      uint32_t m_delta_since_move = 0;

      void m_move();
  };
}
