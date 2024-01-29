#include "./utils.hpp"

#include <array>
#include <cstdint>
#include <random>

namespace
{
std::random_device dev;
std::mt19937 rng(dev());
}  // namespace

namespace dl::utils
{
std::string generate_id()
{
  std::uniform_int_distribution<std::size_t> dist(0, 15);
  const std::string values = "0123456789abcdef";

  const std::array<uint32_t, 16> has_dash = {0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0};
  std::string result{};

  for (int i = 0; i < 16; i++)
  {
    if (has_dash[i] == 1)
    {
      result += "-";
    }

    result += values[dist(rng)];
    result += values[dist(rng)];
  }

  return result;
}
}  // namespace dl::utils
