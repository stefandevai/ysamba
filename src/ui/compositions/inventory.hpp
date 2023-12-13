#pragma once

#include "ui/components/component.hpp"
#include "ui/types.hpp"

namespace dl::ui
{
class ScrollableList;

class Inventory : public UIComponent
{
 public:
  Inventory(const std::function<void(const int i)>& on_select);

  void set_items(const ItemList& items);

 private:
  ScrollableList* m_scrollable_list = nullptr;
};

}  // namespace dl::ui
