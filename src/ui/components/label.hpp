#pragma once

#include "./component.hpp"
#include "graphics/text.hpp"

namespace dl::ui
{
class Label : public UIComponent
{
 public:
  Text text;

  Label(const std::string& text, const std::string& typeface = "font-1980");

  void render(Renderer& renderer, Batch& batch);

 private:
};

}  // namespace dl::ui
