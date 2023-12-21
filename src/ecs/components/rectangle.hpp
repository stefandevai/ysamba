#pragma once

#include "graphics/color.hpp"
#include "graphics/quad.hpp"

namespace dl
{
struct Rectangle
{
  Quad quad;

  Rectangle(int w, int h, const uint32_t color) : quad(Quad{w, h, Color{color}}) {}
};
}  // namespace dl
