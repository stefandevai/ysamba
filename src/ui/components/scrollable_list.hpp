#pragma once

#include "./component.hpp"

namespace dl::ui
{
class ScrollableList : public UIComponent
{
 public:
  ScrollableList(const std::vector<std::string>& items,
                 const Vector2i& size,
                 const std::function<void(const int i)>& on_select);
};

}  // namespace dl::ui
