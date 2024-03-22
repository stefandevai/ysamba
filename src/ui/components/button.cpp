#include "./button.hpp"

#include <spdlog/spdlog.h>

#include "./container.hpp"
#include "./label.hpp"

namespace dl::ui
{
Button::Button(UIContext& context) : UIComponent(context)
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

  if (m_input_manager.has_clicked_bounds(
          InputManager::MouseButton::Left, {absolute_position.x, absolute_position.y}, size))
  {
    on_click();
  }
}

}  // namespace dl::ui
