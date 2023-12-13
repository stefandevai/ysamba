#include "./scrollable.hpp"

#include <spdlog/spdlog.h>

#include "./container.hpp"
#include "./label.hpp"
#include "core/display.hpp"
#include "core/maths/vector.hpp"
#include "graphics/renderer.hpp"

namespace dl::ui
{
Scrollable::Scrollable() : UIComponent()
{
  m_batch.has_depth = false;
  m_batch.has_scissor = true;
}

void Scrollable::update(std::vector<glm::mat4>& matrix_stack)
{
  if (children.empty())
  {
    return;
  }

  if (m_input_manager.is_scrolling_y())
  {
    const auto& mouse_position = m_input_manager.get_mouse_position();

    if (mouse_position.x > absolute_position.x && mouse_position.x < absolute_position.x + size.x &&
        mouse_position.y > absolute_position.y && mouse_position.y < absolute_position.y + size.y)
    {
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

      const auto& matrix = matrix_stack.back();
      matrix_stack.pop_back();
      m_transform_matrix = glm::translate(matrix, glm::vec3(0, m_scroll_y, 0));
      matrix_stack.push_back(m_transform_matrix);
      dirty = true;
    }
  }
}

void Scrollable::render(Renderer& renderer, [[maybe_unused]] Batch& batch)
{
  if (state != UIComponent::State::Visible)
  {
    return;
  }
  if (!m_added_batch)
  {
    renderer.add_batch(&m_batch);
    m_added_batch = true;
  }

  // TODO: Don't use hardcoded layers, impement draw calls objects
  // https://realtimecollisiondetection.net/blog/?p=86
  // https://www.reddit.com/r/opengl/comments/8xzigy/architecture_for_an_opengl_renderer/
  // https://nlguillemot.wordpress.com/2016/11/18/opengl-renderer-design/
  // https://gamedev.stackexchange.com/questions/182241/opengl-rendering-pipeline
  // https://github.com/htmlboss/OpenGL-Renderer/tree/master
  const auto& window_size = Display::get_window_size();
  m_batch.add_scissor({absolute_position.x, window_size.y - absolute_position.y - size.y, size.x, size.y});

  for (auto& child : children)
  {
    child->render(renderer, m_batch);
  }
}

}  // namespace dl::ui
