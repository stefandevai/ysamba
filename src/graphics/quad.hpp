#pragma once

#include "./color.hpp"
#include "./renderable.hpp"

namespace dl
{
struct Quad : public Renderable
{
  Quad(const int w, const int h, const Color& color) : w(w), h(h), color(color) {}
  int w, h;
  Color color;
};
}  // namespace dl
