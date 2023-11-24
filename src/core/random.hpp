#pragma once

#include <random>

namespace dl::random
{
static std::random_device device;
static std::mt19937 rng{device()};
static std::uniform_real_distribution<double> real_distribution{0.f, 1.f};
static std::uniform_int_distribution<int> int_distribution{1, INT_MAX};

static inline double get_real() { return real_distribution(rng); }

static inline int get_integer(const int from = 0, const int to = 100) { return int_distribution(rng) % to + from; }

template <typename T>
static inline T get_value(const std::vector<T>& v)
{
  return v[get_integer(0, v.size() - 1)];
}

template <typename T, typename K>
static inline T get_weighted_value(const std::vector<T>& values, const std::vector<K>& weights)
{
  std::discrete_distribution<> distribution(weights.begin(), weights.end());
  return values[distribution(rng)];
}
}  // namespace dl::random
