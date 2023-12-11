#pragma once

#include "./component.hpp"
#include "ui/types.hpp"

namespace dl::ui
{
class ScrollableList;

class ActionMenu : public UIComponent
{
 public:
  ActionMenu(const ItemList& items, const std::function<void(const uint32_t)>& on_select);
  void set_actions(const ItemList& actions);
  void set_on_select(const std::function<void(const uint32_t)>& on_select);

 private:
  std::shared_ptr<ScrollableList> m_scrollable_list = nullptr;
};

}  // namespace dl::ui
