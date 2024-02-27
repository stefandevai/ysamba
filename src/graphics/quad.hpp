#pragma once

#include "./color.hpp"

namespace dl
{
struct Quad
{
  int w = 0;
  int h = 0;
  Color color;
  int z_index = 0;

  Quad(const int w, const int h, const Color& color, const int z_index = 0) : w(w), h(h), color(color), z_index(z_index)
  {
  }
};
}  // namespace dl
