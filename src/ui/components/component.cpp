#include "./component.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "graphics/batch.hpp"

namespace dl::ui
{
const std::shared_ptr<InputManager> UIComponent::m_input_manager = InputManager::get_instance();

void UIComponent::update(std::vector<glm::mat4>& matrix_stack)
{
  const auto& matrix = matrix_stack.back();
  const auto translate = glm::translate(matrix, glm::vec3(position.x, position.y, position.z));

  if (position.x != 0 && position.y != 0)
  {
    matrix_stack.push_back(translate);
  }

  const auto top_left = translate * glm::vec4(0.f, 0.f, 1.f, 1.f);
  absolute_position.x = top_left.x;
  absolute_position.y = top_left.y;

  for (auto& child : children)
  {
    child->update(matrix_stack);
  }

  if (position.x != 0 && position.y != 0)
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
}  // namespace dl::ui
