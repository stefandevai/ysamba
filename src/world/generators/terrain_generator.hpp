#pragma once

#include <vector>
#include "../../core/lua_api.hpp"
#include "./fast_noise_lite.hpp"

namespace dl
{
  class TerrainGenerator
  {
    public:
      static std::vector<int> generate(const uint32_t width, const uint32_t height, const int seed);
      static float get_ellipse_gradient_value(const uint32_t x, const uint32_t y, const uint32_t width, const uint32_t height);
      static float get_rectangle_gradient_value(const int x, const int y, const int width, const int height);

    private:
      static LuaAPI m_lua;
      static FastNoiseLite m_noise;
  };
}
