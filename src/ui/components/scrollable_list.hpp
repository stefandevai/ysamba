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
  ScrollableList(const ItemList& items, const Vector2i& size, const std::function<void(const uint32_t)>& on_select);
  ScrollableList(const Vector2i& size, const std::function<void(const uint32_t)>& on_select);

  void set_items(const ItemList& items);
  void set_on_select(const std::function<void(const uint32_t)>& on_select);

 private:
  ButtonList* m_list = nullptr;
  Scrollable* m_scrollable = nullptr;
  Container* m_container = nullptr;
  const std::function<void(const uint32_t i)> m_on_select;
};

}  // namespace dl::ui
