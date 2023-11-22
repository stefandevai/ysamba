#pragma once

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
};
}  // namespace dl
