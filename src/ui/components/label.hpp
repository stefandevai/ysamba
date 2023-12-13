#pragma once

#include "./component.hpp"
#include "graphics/text.hpp"

namespace dl::ui
{
class Label : public UIComponent
{
 public:
  Text text;
  Label();
  Label(const std::string_view value);

  void set_text(const std::string_view value);

  void render(Renderer& renderer, Batch& batch);

 private:
};

}  // namespace dl::ui
