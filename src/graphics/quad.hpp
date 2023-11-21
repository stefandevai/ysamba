#pragma once

#include "./color.hpp"
#include "./renderable.hpp"

namespace dl
{
struct Quad : public Renderable
{
  Quad(int w, int h, const Color& color) : w(w), h(h), color(color) {}
  int w, h;
  Color color;
};
}  // namespace dl
