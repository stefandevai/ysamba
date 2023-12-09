#include "./scrollable.hpp"

#include <spdlog/spdlog.h>

#include <entt/core/hashed_string.hpp>

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

  assert(!children[0].expired());

  const auto& matrix = matrix_stack.back();

  if (m_input_manager.is_scrolling_y())
  {
    const auto top_left = matrix * glm::vec4(0.f, 0.f, 1.f, 1.f);
    const auto& mouse_position = m_input_manager.get_mouse_position();

    if (mouse_position.x > top_left.x && mouse_position.x < top_left.x + size.x && mouse_position.y > top_left.y &&
        mouse_position.y < top_left.y + size.y)
    {
      m_scroll_y += m_input_manager.get_scroll().y * 4;

      // Set lower and upper bounds for scrolling
      auto child_ptr = children[0].lock();
      if (m_scroll_y < 0 && size.y < child_ptr->size.y)
      {
        const auto delta_size_y = child_ptr->size.y - size.y;

        if (std::abs(m_scroll_y) > delta_size_y)
        {
          m_scroll_y = -delta_size_y;
        }
      }
      else
      {
        m_scroll_y = 0;
      }
    }
  }

  matrix_stack.pop_back();
  const auto translate = glm::translate(matrix, glm::vec3(0, m_scroll_y, 0));
  matrix_stack.push_back(translate);
}

void Scrollable::render(Renderer& renderer, Batch& batch)
{
  using namespace entt::literals;

  (void)batch;

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
    if (child.expired())
    {
      continue;
    }

    auto child_ptr = child.lock();
    child_ptr->render(renderer, m_batch);
  }
}

}  // namespace dl::ui