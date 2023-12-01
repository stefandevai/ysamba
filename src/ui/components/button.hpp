#pragma once

#include "./component.hpp"

namespace dl::ui
{
class Label;
class Container;

class Button : public UIComponent
{
 public:
  std::function<void()> on_click{};

  Button(const std::string& text,
         const Vector2i& size = {100, 50},
         XAlignement x_alignment = XAlignement::Center,
         YAlignement y_alignment = YAlignement::Center);

  void update(std::vector<glm::mat4>& matrix_stack);

 private:
  std::shared_ptr<Label> m_label = nullptr;
  std::shared_ptr<Container> m_container = nullptr;
};

}  // namespace dl::ui
