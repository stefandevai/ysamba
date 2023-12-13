#include "./button.hpp"

#include <spdlog/spdlog.h>

#include "./container.hpp"
#include "./label.hpp"
#include "graphics/batch.hpp"

namespace dl::ui
{
Button::Button() : UIComponent()
{
  m_container = emplace<Container>();
  m_label = m_container->emplace<Label>();
}

void Button::init()
{
  m_container->set_size(size);
  m_container->set_color(background_color);
  m_label->set_text(text);
  m_label->x_alignment = label_x_alignment;
  m_label->y_alignment = label_y_alignment;
}

void Button::update(const double delta)
{
  (void)delta;

  if (!on_click)
  {
    return;
  }

  if (m_input_manager.has_clicked(InputManager::MouseButton::Left))
  {
    const auto& mouse_position = m_input_manager.get_mouse_position();

    if (mouse_position.x > absolute_position.x && mouse_position.x < absolute_position.x + size.x &&
        mouse_position.y > absolute_position.y && mouse_position.y < absolute_position.y + size.y)
    {
      on_click();
    }
  }
}

}  // namespace dl::ui
