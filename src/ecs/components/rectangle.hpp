#pragma once

#include "graphics/color.hpp"
#include "graphics/quad.hpp"

namespace dl
{
struct Rectangle
{
  Rectangle(int w, int h, const uint32_t color) { quad = std::make_unique<Quad>(w, h, Color{color}); }
  std::unique_ptr<Quad> quad = nullptr;
};
}  // namespace dl
