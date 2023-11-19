#pragma once

#include "./color.hpp"
#include "./renderable.hpp"

namespace dl
{
struct Quad : public Renderable
{
  int w, h;
  Color color;
};
}  // namespace dl
