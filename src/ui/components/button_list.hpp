#pragma once

#include "./component.hpp"
#include "ui/types.hpp"

namespace dl::ui
{
class Button;

template <class T = uint32_t>
class ButtonList : public UIComponent
{
 public:
  int line_spacing = 0;
  /* Vector2i margin = {28, 16}; */
  Vector2i button_size = {25, 25};
  std::function<void(const T)> on_select;

  ButtonList(UIContext& context);

  void init();

  void set_items(const ItemList<T>& items);
  void set_on_select(const std::function<void(const T)>& on_select);

 private:
  ItemList<T> m_items;

  void m_create_buttons();
};

}  // namespace dl::ui
