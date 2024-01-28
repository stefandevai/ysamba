#pragma once

#include <spdlog/spdlog.h>

#include <chrono>
#include <string>

namespace dl
{
class Timer
{
 public:
  std::chrono::time_point<std::chrono::high_resolution_clock> first;
  std::chrono::time_point<std::chrono::high_resolution_clock> second;

  Timer() = default;

  void start();
  void stop();

  template <typename Unit = std::chrono::microseconds>
  void print(const std::string& what)
  {
    const auto duration = std::chrono::duration_cast<Unit>(second - first).count();
    spdlog::info("{} took {} microseconds", what, duration);
  }
};

}  // namespace dl
