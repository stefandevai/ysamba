#pragma once

#include <functional>
#include <string>

#include "./component.hpp"

namespace dl::ui
{
class Label;
class Container;
class MouseRegion;

class FilledButton : public UIComponent
{
 public:
  struct Params
  {
    std::string text{};
    Vector2i size{};
    std::function<void()> on_left_click{};
    std::function<void()> on_right_click{};
    std::function<void()> on_mouse_over{};
    std::function<void()> on_mouse_out{};
    uint32_t text_color = 0xFFFFFFFF;
    uint32_t text_hover_color = 0xEDEDEDEE;
    uint32_t background_color = 0x33AA88AA;
    uint32_t background_hover_color = 0x33AA88CB;
  };

  Label* label = nullptr;
  Container* background = nullptr;
  MouseRegion* mouse_region = nullptr;
  uint32_t text_color{};
  uint32_t text_hover_color{};
  uint32_t background_color{};
  uint32_t background_hover_color{};

  FilledButton(UIContext& context, Params params);
};

}  // namespace dl::ui
