#include "./button.hpp"

#include <spdlog/spdlog.h>

#include "./container.hpp"
#include "./label.hpp"

namespace dl::ui
{
Button::Button(UIContext& context) : UIComponent(context)
{
  size = {160, 32};
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

void Button::update()
{
  if (!on_click)
  {
    return;
  }

  if (m_input_manager.has_clicked_bounds(
          InputManager::MouseButton::Left, {absolute_position.x, absolute_position.y}, size))
  {
    on_click();
  }
}

}  // namespace dl::ui
