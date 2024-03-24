#pragma once

#include "ui/components/component.hpp"
#include "ui/types.hpp"

namespace dl::ui
{
template <class T>
class ScrollableList;
class WindowFrame;

class SocietyInventory : public UIComponent
{
 public:
  SocietyInventory(UIContext& context, const std::function<void(const int i)>& on_select);

  void set_items(const ItemList<uint32_t>& items);
  void show();
  void hide();

 private:
  ScrollableList<uint32_t>* m_items = nullptr;
  WindowFrame* m_window_frame = nullptr;
};

}  // namespace dl::ui
