#include "./component.hpp"

#include <spdlog/spdlog.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "graphics/display.hpp"

namespace dl::ui
{
void UIComponent::m_init()
{
  init();

  for (const auto& child : children)
  {
    if (!child->has_initialized)
    {
      child->m_init();
    }
  }

  has_initialized = true;
  dirty = true;
}

void UIComponent::m_after_init()
{
  after_init();

  for (const auto& child : children)
  {
    child->m_after_init();
  }
}

void UIComponent::m_process_input()
{
  if (state != State::Visible)
  {
    return;
  }

  process_input();

  for (const auto& child : children)
  {
    child->m_process_input();
  }
}

void UIComponent::m_update_geometry()
{
  if (!is_active())
  {
    return;
  }

  if (dirty)
  {
    const auto& window_size = Display::get_window_size();
    const auto& matrix = m_context.matrix_stack->back();

    glm::vec3 transformed_position{position.x + margin.x, position.y + margin.y, position.z};

    if (x_alignment == XAlignement::Right)
    {
      const int anchor_w = (parent == nullptr || parent->size.x == 0) ? window_size.x : parent->size.x;
      transformed_position.x = anchor_w - size.x - position.x - margin.x;
    }
    else if (x_alignment == XAlignement::Center)
    {
      const int anchor_w = (parent == nullptr || parent->size.x == 0) ? window_size.x : parent->size.x;
      transformed_position.x = anchor_w / 2 - size.x / 2 + position.x + margin.x;
    }

    if (y_alignment == YAlignement::Bottom)
    {
      const int anchor_h = (parent == nullptr || parent->size.y == 0) ? window_size.y : parent->size.y;
      transformed_position.y = anchor_h - size.y - position.y - margin.y;
    }
    else if (y_alignment == YAlignement::Center)
    {
      const int anchor_h = (parent == nullptr || parent->size.y == 0) ? window_size.y : parent->size.y;
      transformed_position.y = anchor_h / 2 - size.y / 2 + position.y + margin.y;
    }

    // Increase z-index if the component is renderable so that children will be rendered on top of their parents
    if (is_renderable)
    {
      transformed_position.z += 1;
    }

    m_transform_matrix
        = glm::translate(matrix, glm::vec3(transformed_position.x, transformed_position.y, transformed_position.z));
    const auto top_left = m_transform_matrix * glm::vec4(0.f, 0.f, 0.f, 1.f);
    absolute_position.x = top_left.x;
    absolute_position.y = top_left.y;
    absolute_position.z = top_left.z;
  }

  m_context.min_z_index = std::min(m_context.min_z_index, absolute_position.z);
  m_context.max_z_index = std::max(m_context.max_z_index, absolute_position.z);
}

void UIComponent::m_update()
{
  if (!has_initialized)
  {
    m_init();
    m_after_init();
  }

  m_update_geometry();

  if (m_is_positioned())
  {
    m_context.matrix_stack->push_back(m_transform_matrix);
  }

  if (state == State::Visible)
  {
    update();
  }

  // // If dirty, propagate to parents
  // if (dirty)
  // {
  //   auto current_parent = parent;
  //
  //   while (current_parent != nullptr)
  //   {
  //     current_parent->dirty = dirty;
  //     current_parent = current_parent->parent;
  //   }
  // }

  for (const auto& child : children)
  {
    // If dirty, propagate to children
    if (dirty)
    {
      child->dirty = dirty;
    }

    child->opacity = opacity;
    child->m_update();
  }

  if (m_is_positioned())
  {
    m_context.matrix_stack->pop_back();
  }

  dirty = false;
}

void UIComponent::render()
{
  if (!is_active())
  {
    return;
  }

  for (const auto& child : children)
  {
    child->render();
  }
}

void UIComponent::show()
{
  state = State::Visible;
}

void UIComponent::hide()
{
  state = State::Hidden;
}

void UIComponent::force_hide()
{
  state = State::Hidden;
}

void UIComponent::propagate_state()
{
  for (auto& child : children)
  {
    child->state = state;
    child->propagate_state();
  }
}

bool UIComponent::is_active()
{
  return state != State::Hidden && has_initialized;
}

bool UIComponent::is_hidden()
{
  return state == State::Hidden;
}

bool UIComponent::is_visible()
{
  return state == State::Visible;
}

bool UIComponent::m_is_positioned()
{
  return is_renderable || position.x != 0 || position.y != 0 || position.z != 0 || x_alignment != XAlignement::Left
         || y_alignment != YAlignement::Top;
}
}  // namespace dl::ui
