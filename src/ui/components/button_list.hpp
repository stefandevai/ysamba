#pragma once

#include "./component.hpp"
#include "ui/style/list.hpp"
#include "ui/types.hpp"

namespace dl::ui
{
class Button;

class ButtonList : public UIComponent
{
 public:
  ListStyle style;
  std::function<void(const uint32_t)> on_select;

  ButtonList(const ItemList& items,
             const Vector2i& button_size,
             const std::function<void(const uint32_t)>& on_select,
             const ListStyle& style = ListStyle());

  void set_on_select(const std::function<void(const uint32_t)>& on_select);

 private:
  std::vector<std::shared_ptr<Button>> m_buttons;
};

}  // namespace dl::ui
