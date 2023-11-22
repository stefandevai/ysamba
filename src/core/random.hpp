#pragma once

#include <random>

namespace dl::random
{
static std::random_device device;
static std::mt19937 rng{device()};
static std::uniform_real_distribution<double> real_distribution{0.f, 1.f};
static std::uniform_int_distribution<int> int_distribution{1, INT_MAX};

inline double get_real() { return real_distribution(rng); }

inline int get_integer(const int from = 0, const int to = 100) { return int_distribution(rng) % to + from; }
}  // namespace dl::random
