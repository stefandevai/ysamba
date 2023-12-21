#pragma once

#include "./component.hpp"
#include "graphics/text.hpp"

namespace dl::ui
{
class Label : public UIComponent
{
 public:
  Text text;
  std::string value;
  bool wrap = true;

  Label(UIContext& context);
  Label(UIContext& context, const std::string_view value);

  void init();
  void render(Batch& batch);

  void set_text(const std::string_view value);

 private:
};

}  // namespace dl::ui
