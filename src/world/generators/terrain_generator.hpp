#pragma once

#include <vector>
#include "./fast_noise_lite.hpp"

namespace dl
{
  class TerrainGenerator
  {
    public:
      static std::vector<int> generate(const uint32_t width, const uint32_t height);
      static float get_ellipse_gradient_value(const uint32_t x, const uint32_t y, const uint32_t width, const uint32_t height);
      static float get_rectangle_gradient_value(const uint32_t x, const uint32_t y, const uint32_t width, const uint32_t height);

    private:
      static FastNoiseLite m_noise;
  };
}
