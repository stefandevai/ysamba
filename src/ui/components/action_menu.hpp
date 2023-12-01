#pragma once

#include "./component.hpp"

namespace dl::ui
{
class ActionMenu : public UIComponent
{
 public:
  ActionMenu(const std::vector<std::string>& items, const std::function<void(const int i)>& on_select);
};

}  // namespace dl::ui
