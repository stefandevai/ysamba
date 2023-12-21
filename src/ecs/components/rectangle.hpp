#pragma once

#include "graphics/color.hpp"
#include "graphics/quad.hpp"

namespace dl
{
struct Rectangle
{
  Quad quad;
  int z_index = 0;

  Rectangle(const int w, const int h, const uint32_t color, const int z_index = 0)
      : quad(Quad{w, h, Color{color}}), z_index(z_index)
  {
  }
};
}  // namespace dl
