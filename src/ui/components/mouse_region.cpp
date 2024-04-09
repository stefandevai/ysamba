#include "./mouse_region.hpp"

#include <spdlog/spdlog.h>

namespace dl::ui
{
MouseRegion::MouseRegion(UIContext& context, Params params)
    : UIComponent(context, "MouseRegion"),
      on_left_click(std::move(params.on_left_click)),
      on_right_click(std::move(params.on_right_click)),
      on_mouse_over(std::move(params.on_mouse_over)),
      on_mouse_out(std::move(params.on_mouse_out))
{
}

void MouseRegion::after_init()
{
  // TODO: Compute size occupied by all children
  if (!children.empty())
  {
    size = children[0]->size;
  }
}

void MouseRegion::process_input()
{
  if (m_input_manager.mouse_hover_aabb(absolute_position.xy(), size))
  {
    m_last_mouse_over = true;
    m_input_manager.set_mouse_cursor(MouseCursor::Hand);

    if (on_left_click && m_input_manager.has_clicked(InputManager::MouseButton::Left))
    {
      on_left_click();
    }
    else if (on_right_click && m_input_manager.has_clicked(InputManager::MouseButton::Right))
    {
      on_right_click();
    }
    else if (on_mouse_over)
    {
      on_mouse_over();
    }
  }
  else if (m_last_mouse_over)
  {
    if (on_mouse_out)
    {
      on_mouse_out();
    }

    m_last_mouse_over = false;
  }
}
}  // namespace dl::ui
