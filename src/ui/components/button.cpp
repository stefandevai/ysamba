#include "./button.hpp"

#include <spdlog/spdlog.h>

#include "./container.hpp"
#include "./label.hpp"
#include "graphics/batch.hpp"

namespace dl::ui
{
Button::Button(const std::string& text, const Vector2i& size, XAlignement x_alignment, YAlignement y_alignment)
    : UIComponent(size)
{
  m_container = emplace<Container>(size, 0x33aa88aa);
  m_label = m_container->emplace<Label>(text);
  m_label->parent_size = size;
  m_label->x_alignment = x_alignment;
  m_label->y_alignment = y_alignment;
}

void Button::update(std::vector<glm::mat4>& matrix_stack)
{
  if (!on_click)
  {
    return;
  }

  if (m_input_manager.has_clicked(InputManager::MouseButton::Left))
  {
    const auto& matrix = matrix_stack.back();
    const auto top_left = matrix * glm::vec4(0.f, 0.f, 1.f, 1.f);
    const auto& mouse_position = m_input_manager.get_mouse_position();

    if (mouse_position.x > top_left.x && mouse_position.x < top_left.x + size.x && mouse_position.y > top_left.y &&
        mouse_position.y < top_left.y + size.y)
    {
      on_click();
    }
  }
}

}  // namespace dl::ui
