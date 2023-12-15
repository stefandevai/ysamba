#pragma once

#include "./component.hpp"
#include "graphics/text.hpp"

namespace dl::ui
{
class Label : public UIComponent
{
 public:
  Text text;
  Label(UIContext& context);
  Label(UIContext& context, const std::string_view value);

  void set_text(const std::string_view value);

  void render(Batch& batch);

 private:
};

}  // namespace dl::ui
