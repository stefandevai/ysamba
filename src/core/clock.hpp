#pragma once

#include <chrono>

namespace dl
{
struct Clock
{
  uint32_t frame = 0;
  /* uint32_t last_tick_time = 0; */
  uint32_t delta_ms = 0;
  double delta = 0;
  std::chrono::steady_clock::time_point last_time = std::chrono::steady_clock::now();

  void tick()
  {
    auto now = std::chrono::steady_clock::now();
    delta_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_time).count();
    /* uint32_t tick_time = SDL_GetTicks(); */
    /* delta_ms = tick_time - last_tick_time; */
    delta = delta_ms / 1000.0;
    /* last_tick_time = tick_time; */
    last_time = now;
    ++frame;
  }
};
}  // namespace dl
