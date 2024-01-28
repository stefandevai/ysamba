#include "./timer.hpp"

namespace dl
{
void Timer::start() { first = std::chrono::high_resolution_clock::now(); }

void Timer::stop() { second = std::chrono::high_resolution_clock::now(); }
}  // namespace dl
