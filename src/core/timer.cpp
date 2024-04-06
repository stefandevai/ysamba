#include "./timer.hpp"

namespace dl
{
void Timer::start()
{
  first = std::chrono::high_resolution_clock::now();
  is_running = true;
}

void Timer::stop()
{
  second = std::chrono::high_resolution_clock::now();
  is_running = false;
}
}  // namespace dl
