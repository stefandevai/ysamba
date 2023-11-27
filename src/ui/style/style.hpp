#pragma once

#include "graphics/color.hpp"

namespace dl::ui
{
struct Style
{
  Color color;
  virtual ~Style() {}
};
}  // namespace dl::ui
