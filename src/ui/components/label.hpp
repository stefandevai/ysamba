#pragma once

#include "./component.hpp"
#include "graphics/text.hpp"

namespace dl::ui
{
struct LabelParams
{
  std::string value{};
  bool wrap = true;
  uint32_t color = 0xFFFFFFFF;
};

class Label : public UIComponent
{
 public:
  Text text{};
  std::string value;
  bool wrap = true;

  Label(UIContext& context);
  Label(UIContext& context, const std::string_view value);
  Label(UIContext& context, LabelParams params);

  void init();
  void render();

  void set_text(const std::string_view value);
  void set_color(uint32_t color);
};

}  // namespace dl::ui
