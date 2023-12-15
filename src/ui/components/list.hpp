#pragma once

#include "./component.hpp"
#include "core/maths/vector.hpp"

namespace dl::ui
{
class List : public UIComponent
{
 public:
  int line_spacing = 5;
  Vector2i margin = {15, 15};

  List(UIContext& context, const std::vector<std::string>& items);

  void init();

 private:
  std::vector<std::string> m_items;
};

}  // namespace dl::ui
