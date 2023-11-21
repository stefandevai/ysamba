#pragma once

#include <random>

#include "../tilemap.hpp"

namespace dl
{
class DummyGenerator
{
 public:
  DummyGenerator(const int width, const int height) : m_width(width), m_height(height) {}

  Tilemap generate(const int seed);

 private:
  const int m_width;
  const int m_height;
  std::random_device m_rd;
  std::mt19937 m_rng{m_rd()};
  std::uniform_real_distribution<float> m_distribution{0.f, 1.f};
};
}  // namespace dl
