#include "./component.hpp"

#include <spdlog/spdlog.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "core/display.hpp"
#include "graphics/renderer.hpp"

namespace dl::ui
{
InputManager& UIComponent::m_input_manager = InputManager::get_instance();

void UIComponent::update_component(std::vector<glm::mat4>& matrix_stack)
{
  if (!visible)
  {
    return;
  }

  if (dirty)
  {
    const auto& window_size = Display::get_window_size();
    const auto& matrix = matrix_stack.back();

    glm::vec3 transformed_position{position.x, position.y, position.z};

    if (x_alignment == XAlignement::Right)
    {
      const int anchor_w = parent_size.x == 0 ? window_size.x : parent_size.x;
      transformed_position.x = anchor_w - size.x - position.x;
    }
    else if (x_alignment == XAlignement::Center)
    {
      const int anchor_w = parent_size.x == 0 ? window_size.x : parent_size.x;
      transformed_position.x = anchor_w / 2 - size.x / 2 + position.x;
    }
    if (y_alignment == YAlignement::Bottom)
    {
      const int anchor_h = parent_size.y == 0 ? window_size.y : parent_size.y;
      transformed_position.y = anchor_h - size.y - position.y;
    }
    else if (y_alignment == YAlignement::Center)
    {
      const int anchor_h = parent_size.y == 0 ? window_size.y : parent_size.y;
      transformed_position.y = anchor_h / 2 - size.y / 2 + position.y;
    }
    m_transform_matrix =
        glm::translate(matrix, glm::vec3(transformed_position.x, transformed_position.y, transformed_position.z));
    const auto top_left = m_transform_matrix * glm::vec4(0.f, 0.f, 1.f, 1.f);
    absolute_position.x = top_left.x;
    absolute_position.y = top_left.y;
  }
  if (m_is_positioned())
  {
    matrix_stack.push_back(m_transform_matrix);
  }

  update(matrix_stack);

  for (const auto& child : children)
  {
    child->dirty = dirty;
    child->update_component(matrix_stack);
  }

  if (m_is_positioned())
  {
    matrix_stack.pop_back();
  }

  dirty = false;
}

void UIComponent::render(Renderer& renderer, Batch& batch)
{
  if (!visible)
  {
    return;
  }

  for (const auto& child : children)
  {
    child->render(renderer, batch);
  }
}

void UIComponent::show() { visible = true; }

void UIComponent::hide() { visible = false; }

bool UIComponent::m_is_positioned()
{
  return position.x != 0 || position.y != 0 || x_alignment != XAlignement::Left || y_alignment != YAlignement::Top;
}
}  // namespace dl::ui
