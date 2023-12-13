#pragma once

#include "./color.hpp"

namespace dl
{
struct Quad
{
  int w, h;
  Color color;
  Quad(const int w, const int h, const Color& color) : w(w), h(h), color(color) {}
};
}  // namespace dl
