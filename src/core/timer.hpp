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
  std::size_t count()
  {
    return std::chrono::duration_cast<Unit>(second - first).count();
  }

  template <typename Unit = std::chrono::microseconds>
  void print(const std::string& what)
  {
    const auto duration = std::chrono::duration_cast<Unit>(second - first).count();
    spdlog::info("{} took {} microseconds", what, duration);
  }

  // Prints the duration once after the given interval in seconds
  template <typename Unit = std::chrono::microseconds>
  void print(const std::string& what, const double interval)
  {
    const auto now = std::chrono::high_resolution_clock::now();
    const auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - last_printed).count();

    if (elapsed < interval)
    {
      return;
    }

    print<Unit>(what);
    last_printed = now;
  }

 private:
  std::chrono::time_point<std::chrono::high_resolution_clock> last_printed;
};

}  // namespace dl
