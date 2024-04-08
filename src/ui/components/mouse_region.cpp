#include "./mouse_region.hpp"

#include <spdlog/spdlog.h>

namespace dl::ui
{
MouseRegion::MouseRegion(UIContext& context, MouseRegionParams params)
    : UIComponent(context),
      on_left_click(std::move(params.on_left_click)),
      on_right_click(std::move(params.on_right_click)),
      on_hover(std::move(params.on_hover))
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
    m_input_manager.set_mouse_cursor(MouseCursor::Hand);

    if (on_left_click && m_input_manager.has_clicked(InputManager::MouseButton::Left))
    {
      on_left_click();
    }
    else if (on_right_click && m_input_manager.has_clicked(InputManager::MouseButton::Right))
    {
      on_right_click();
    }
    else if (on_hover)
    {
      on_hover();
    }
  }
}
}  // namespace dl::ui
