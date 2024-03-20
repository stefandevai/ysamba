#pragma once

#include "./component.hpp"
#include "ui/types.hpp"

namespace dl::ui
{
template <class T>
class ButtonList;
class Scrollable;
class WindowFrame;
class Label;

template <class T = uint32_t>
class ScrollableList : public UIComponent
{
 public:
  uint32_t color = 0x1b2420aa;
  std::string title{};
  ButtonList<T>* list = nullptr;
  Scrollable* scrollable = nullptr;

  ScrollableList(UIContext& context);

  void init();
  void set_items(const ItemList<T>& items);
  void set_on_select(const std::function<void(const T)>& on_select);
  void reset_scroll();

 private:
  Label* m_title_label = nullptr;
  std::function<void(const T i)> m_on_select;
  ItemList<T> m_items;
};

}  // namespace dl::ui
