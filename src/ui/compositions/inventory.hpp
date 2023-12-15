#pragma once

#include "ui/components/component.hpp"
#include "ui/types.hpp"

namespace dl::ui
{
class ScrollableList;
class WindowFrame;

class Inventory : public UIComponent
{
 public:
  Inventory(UIContext& context, const std::function<void(const int i)>& on_select);

  void set_items(const ItemList& items);
  void show();
  void hide();

 private:
  ScrollableList* m_scrollable_list = nullptr;
  WindowFrame* m_window_frame = nullptr;
};

}  // namespace dl::ui
