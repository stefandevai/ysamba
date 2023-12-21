#pragma once

#include "./color.hpp"

namespace dl
{
struct Quad
{
  int w = 0;
  int h = 0;
  Color color;

  Quad(const int w, const int h, const Color& color) : w(w), h(h), color(color) {}
};
}  // namespace dl
