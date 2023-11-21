#pragma once

#include "../../graphics/color.hpp"
#include "../../graphics/quad.hpp"

namespace dl
{
struct Rectangle
{
  Rectangle(int w, int h, const std::string& color) { quad = std::make_shared<Quad>(w, h, Color{color}); }
  std::shared_ptr<Quad> quad = nullptr;
};
}  // namespace dl
