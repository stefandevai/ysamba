#pragma once

#include "./component.hpp"
#include "ui/types.hpp"

namespace dl::ui
{
class ButtonList;
class Scrollable;
class Container;

class ScrollableList : public UIComponent
{
 public:
  uint32_t color = 0x1b2420aa;

  ScrollableList(UIContext& context);

  void init();
  void set_items(const ItemList& items);
  void set_on_select(const std::function<void(const uint32_t)>& on_select);

 private:
  ButtonList* m_list = nullptr;
  Scrollable* m_scrollable = nullptr;
  Container* m_container = nullptr;
  std::function<void(const uint32_t i)> m_on_select;
  ItemList m_items;
};

}  // namespace dl::ui
