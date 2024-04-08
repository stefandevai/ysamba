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
  std::function<void()> on_hover{};
};

class TextButton : public UIComponent
{
 public:
  Label* label = nullptr;

  TextButton(UIContext& context, TextButtonParams params);
};

}  // namespace dl::ui
