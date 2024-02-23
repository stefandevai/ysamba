#include "./scrollable.hpp"

#include <spdlog/spdlog.h>

#include "./container.hpp"
#include "./label.hpp"
#include "core/display.hpp"
#include "core/maths/vector.hpp"
#include "graphics/renderer/renderer.hpp"

namespace dl::ui
{
Scrollable::Scrollable(UIContext& context) : UIComponent(context)
{
  // m_batch.has_depth = false;
  // m_batch.has_scissor = true;
  // m_context.renderer->add_batch(&m_batch);
}

void Scrollable::update()
{
  if (children.empty())
  {
    return;
  }

  if (m_input_manager.is_scrolling_y())
  {
    const auto& mouse_position = m_input_manager.get_mouse_position();

    if (mouse_position.x > absolute_position.x && mouse_position.x < absolute_position.x + size.x
        && mouse_position.y > absolute_position.y && mouse_position.y < absolute_position.y + size.y)
    {
      assert(children.size() == 1 && "Updating scroll with a number of children different than 1");
      const auto& child = children[0];
      m_scroll_y += m_input_manager.get_scroll().y * 4;

      // Set lower and upper bounds for scrolling
      if (m_scroll_y < 0 && size.y < child->size.y)
      {
        const auto delta_size_y = child->size.y - size.y;

        if (std::abs(m_scroll_y) > delta_size_y)
        {
          m_scroll_y = -delta_size_y;
        }
      }
      else
      {
        m_scroll_y = 0;
      }

      child->position.y = m_scroll_y;
      dirty = true;
    }
  }
}

void Scrollable::render()
{
  if (state == UIComponent::State::Hidden)
  {
    return;
  }

  const auto& window_size = Display::get_window_size();
  m_context.renderer->batch.push_scissor(
      {absolute_position.x, window_size.y - absolute_position.y - size.y, size.x, size.y});

  for (auto& child : children)
  {
    child->render();
  }

  m_context.renderer->batch.pop_scissor();
}

void Scrollable::reset_scroll()
{
  assert(children.size() == 1 && "Reseting scroll with a number of children different than 1");

  const auto& child = children[0];

  m_scroll_y = 0;
  child->position.y = 0;
  dirty = true;
}

}  // namespace dl::ui
