#pragma once

#include "./component.hpp"
#include "ui/style/list.hpp"

namespace dl::ui
{
class List : public UIComponent
{
 public:
  List(const std::vector<std::string>& items, const ListStyle& style = ListStyle());

  ListStyle style;
};

}  // namespace dl::ui
