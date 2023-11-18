#pragma once

#include "./renderable.hpp"
#include "./color.hpp"

namespace dl
{
  struct Quad : public Renderable
  {
    Quad() {}
    Color color;
    int w, h;
  };
}


