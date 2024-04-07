#pragma once

#include "./component.hpp"

namespace dl::ui
{
class Label;
class Container;

class Button : public UIComponent
{
 public:
  uint32_t background_color = 0x33aa88aa;
  uint32_t color = 0xCCC1AFFF;
  uint32_t hover_color = 0xFFFFFFFF;

  std::string text{};
  std::function<void()> on_click{};
  XAlignement label_x_alignment = XAlignement::Center;
  YAlignement label_y_alignment = YAlignement::Center;

  Label* label = nullptr;
  Container* container = nullptr;

  Button(UIContext& context);

  void init();
  void update();
};

}  // namespace dl::ui
