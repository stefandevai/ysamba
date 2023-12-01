#pragma once

#include "./component.hpp"

namespace dl::ui
{
class ButtonList;
class Scrollable;
class Container;

class ScrollableList : public UIComponent
{
 public:
  ScrollableList(const std::vector<std::string>& items,
                 const Vector2i& size,
                 const std::function<void(const int i)>& on_select);

 private:
  std::shared_ptr<ButtonList> m_list = nullptr;
  std::shared_ptr<Scrollable> m_scrollable = nullptr;
  std::shared_ptr<Container> m_container = nullptr;
};

}  // namespace dl::ui
