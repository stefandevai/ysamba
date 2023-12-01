#pragma once

#include "./component.hpp"

namespace dl::ui
{
class Button : public UIComponent
{
 public:
  std::function<void()> on_click{};

  Button(const std::string& text,
         const Vector2i& size = {100, 50},
         XAlignement x_alignment = XAlignement::Center,
         YAlignement y_alignment = YAlignement::Center);

  void update(std::vector<glm::mat4>& matrix_stack);
};

}  // namespace dl::ui
