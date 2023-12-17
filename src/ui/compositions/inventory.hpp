#pragma once

#include "ui/components/component.hpp"
#include "ui/types.hpp"

namespace dl::ui
{
template <class T>
class ScrollableList;
class WindowFrame;

class Inventory : public UIComponent
{
 public:
  Inventory(UIContext& context, const std::function<void(const int i)>& on_select);

  void set_weared_items(const ItemList<uint32_t>& items);
  void set_carried_items(const ItemList<uint32_t>& items);
  void show();
  void hide();

 private:
  ScrollableList<uint32_t>* m_weared_items = nullptr;
  ScrollableList<uint32_t>* m_carried_items = nullptr;
  WindowFrame* m_window_frame = nullptr;
};

}  // namespace dl::ui
