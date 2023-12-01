#pragma once

#include "./component.hpp"

namespace dl::ui
{
class ScrollableList;

class ActionMenu : public UIComponent
{
 public:
  ActionMenu(const std::vector<std::string>& items, const std::function<void(const int i)>& on_select);

 private:
  std::shared_ptr<ScrollableList> m_scrollable_list = nullptr;
};

}  // namespace dl::ui
