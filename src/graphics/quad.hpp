#pragma once

#include "./color.hpp"

namespace dl
{
struct Quad
{
  Quad(const int w, const int h, const Color& color) : w(w), h(h), color(color) {}
  int w, h;
  Color color;
};
}  // namespace dl
