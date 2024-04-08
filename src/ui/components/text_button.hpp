#pragma once

#include <functional>
#include <string>

#include "./component.hpp"

namespace dl::ui
{
class Label;

struct TextButtonParams
{
  std::string text{};
  std::function<void()> on_left_click{};
  std::function<void()> on_right_click{};
  std::function<void()> on_mouse_over{};
  std::function<void()> on_mouse_out{};
  uint32_t text_color = 0xFFFFFFFF;
  uint32_t text_hover_color = 0xFFFFFFFF;
};

class TextButton : public UIComponent
{
 public:
  Label* label = nullptr;
  uint32_t text_color{};
  uint32_t text_hover_color{};

  TextButton(UIContext& context, TextButtonParams params);
};

}  // namespace dl::ui
