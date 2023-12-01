#pragma once

#include "./component.hpp"
#include "ui/style/list.hpp"

namespace dl::ui
{
class Button;

class ButtonList : public UIComponent
{
 public:
  ButtonList(const std::vector<std::string>& items,
             const Vector2i& button_size,
             std::function<void(const int)> on_select,
             const ListStyle& style = ListStyle());

  ListStyle style;
  std::function<void(const int)> on_select;

 private:
  std::vector<std::shared_ptr<Button>> m_buttons;
};

}  // namespace dl::ui
