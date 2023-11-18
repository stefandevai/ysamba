#pragma once

#include "./renderable.hpp"
#include "./color.hpp"

namespace dl
{
  struct Quad : public Renderable
  {
    int w, h;
    Color color;
  };
}


