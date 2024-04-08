#include "./button.hpp"

#include <spdlog/spdlog.h>

#include "ui/components/container.hpp"
#include "ui/components/label.hpp"

namespace dl::ui
{
Button::Button(UIContext& context) : UIComponent(context, "Button")
{
  size = Vector2i{160, 32};
  container = emplace<Container>();
  label = container->emplace<Label>();
}

void Button::init()
{
  container->set_size(size);
  container->set_color(background_color);
  label->set_text(text);
  label->x_alignment = label_x_alignment;
  label->y_alignment = label_y_alignment;
}

void Button::update()
{
  if (!on_click)
  {
    return;
  }

  if (m_input_manager.mouse_hover_aabb(absolute_position.xy(), size))
  {
    m_input_manager.set_mouse_cursor(MouseCursor::Hand);
    label->set_color(hover_color);
  }
  else
  {
    label->set_color(color);
  }

  if (m_input_manager.has_clicked_aabb(InputManager::MouseButton::Left, absolute_position.xy(), size))
  {
    on_click();
  }
}

}  // namespace dl::ui
