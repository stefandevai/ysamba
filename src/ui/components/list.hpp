#pragma once

#include "./component.hpp"

namespace dl::ui
{
class List : public UIComponent
{
 public:
  List();

  void update(std::vector<glm::mat4>& matrix_stack);

 private:
  int m_scroll_y = 0;
};

}  // namespace dl::ui
