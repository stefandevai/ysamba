#pragma once

#include "./component.hpp"

namespace dl::ui
{
class Label;
class Container;

class Button : public UIComponent
{
 public:
  uint32_t background_color = 0x33aa88aa;
  std::string text{};
  std::function<void()> on_click{};
  XAlignement label_x_alignment = XAlignement::Center;
  YAlignement label_y_alignment = YAlignement::Center;

  Button() = default;
  /* Button(const std::string& text, */
  /*        const Vector2i& size = {100, 50}, */
  /*        XAlignement x_alignment = XAlignement::Center, */
  /*        YAlignement y_alignment = YAlignement::Center); */

  void init();
  void update(std::vector<glm::mat4>& matrix_stack);

 private:
  Label* m_label = nullptr;
  Container* m_container = nullptr;
};

}  // namespace dl::ui
