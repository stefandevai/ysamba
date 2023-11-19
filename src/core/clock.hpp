#pragma once

#include <SDL.h>

namespace dl
{
struct Clock
{
  uint32_t last_tick_time = 0;
  uint32_t delta_ms = 0;
  double delta = 0;

  void tick()
  {
    uint32_t tick_time = SDL_GetTicks();
    delta_ms = tick_time - last_tick_time;
    delta = delta_ms / 1000.0;
    last_tick_time = tick_time;
  }
};
}  // namespace dl
