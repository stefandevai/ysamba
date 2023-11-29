#include "./component.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "core/display.hpp"
#include "graphics/batch.hpp"

namespace dl::ui
{
const std::shared_ptr<InputManager> UIComponent::m_input_manager = InputManager::get_instance();

void UIComponent::update_component(std::vector<glm::mat4>& matrix_stack)
{
  const auto& window_size = Display::get_window_size();
  const auto& matrix = matrix_stack.back();

  glm::vec3 transformed_position{position.x, position.y, position.z};

  if (x_alignment == XAlignement::Right)
  {
    transformed_position.x = window_size.x - size.x - position.x;
  }
  else if (x_alignment == XAlignement::Center)
  {
    transformed_position.x = window_size.x / 2 - size.x / 2 + position.x;
  }
  if (y_alignment == YAlignement::Bottom)
  {
    transformed_position.y = window_size.y - size.y - position.y;
  }
  else if (y_alignment == YAlignement::Center)
  {
    transformed_position.y = window_size.y / 2 - size.y / 2 + position.y;
  }
  const auto translate =
      glm::translate(matrix, glm::vec3(transformed_position.x, transformed_position.y, transformed_position.z));
  const auto top_left = translate * glm::vec4(0.f, 0.f, 1.f, 1.f);
  absolute_position.x = top_left.x;
  absolute_position.y = top_left.y;

  if (m_is_positioned())
  {
    matrix_stack.push_back(translate);
  }

  update(matrix_stack);

  for (auto& child : children)
  {
    child->update_component(matrix_stack);
  }

  if (m_is_positioned())
  {
    matrix_stack.pop_back();
  }
}

void UIComponent::render(const std::shared_ptr<Batch> batch)
{
  for (auto& child : children)
  {
    child->render(batch);
  }
}

bool UIComponent::m_is_positioned()
{
  return position.x != 0 || position.y != 0 || x_alignment != XAlignement::Left || y_alignment != YAlignement::Top;
}
}  // namespace dl::ui
