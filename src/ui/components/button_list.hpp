#pragma once

#include "./component.hpp"
#include "ui/types.hpp"

namespace dl::ui
{
class Button;

class ButtonList : public UIComponent
{
 public:
  int line_spacing = 0;
  Vector2i margin = {28, 16};
  Vector2i button_size = {25, 25};
  std::function<void(const uint32_t)> on_select;

  ButtonList() = default;

  void set_items(const ItemList& items);
  void set_on_select(const std::function<void(const uint32_t)>& on_select);

  void init();

 private:
  ItemList m_items;
  std::vector<Button*> m_buttons;
};

}  // namespace dl::ui
