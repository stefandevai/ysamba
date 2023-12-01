#pragma once

#include "./component.hpp"

namespace dl::ui
{
class Scrollable : public UIComponent
{
 public:
  Scrollable();

  void update(std::vector<glm::mat4>& matrix_stack);
  void render(Renderer& renderer, const std::string& layer);

 private:
  int m_scroll_y = 0;
};

}  // namespace dl::ui
