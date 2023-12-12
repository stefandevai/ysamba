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
  int line_spacing = 5;
  Vector2i margin = {15, 15};
  Vector2i button_size = {35, 35};
  std::function<void(const uint32_t)> on_select;

  ButtonList(const ItemList& items, const std::function<void(const uint32_t)>& on_select);

  void set_on_select(const std::function<void(const uint32_t)>& on_select);

  void init();

 private:
  ItemList m_items;
  std::vector<Button*> m_buttons;
};

}  // namespace dl::ui
