#pragma once

#include "ui/components/component.hpp"
#include "ui/types.hpp"

namespace dl::ui
{
template <class T>
class ScrollableList;
class WindowFrame;

class ActionMenu : public UIComponent
{
 public:
  ActionMenu(UIContext& context, const ItemList<uint32_t>& items, const std::function<void(const uint32_t)>& on_select);
  void set_actions(const ItemList<uint32_t>& actions);
  void set_on_select(const std::function<void(const uint32_t)>& on_select);
  void show();
  void hide();

 private:
  ScrollableList<uint32_t>* m_scrollable_list = nullptr;
  WindowFrame* m_window_frame = nullptr;
};

}  // namespace dl::ui
