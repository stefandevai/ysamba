#include "./button.hpp"

#include <spdlog/spdlog.h>

#include "./container.hpp"
#include "./label.hpp"
#include "graphics/batch.hpp"

namespace dl::ui
{
Button::Button(const std::string& text, const Vector3i& position, const Vector2i& size)
    : UIComponent(ComponentType::Quad), m_size(size)
{
  this->position = position;
  m_size = size;

  auto container = std::make_shared<Container>(m_size.x, m_size.y, "#33aa88aa");
  auto label = std::make_shared<Label>(text);
  const auto& label_size = label->text.get_size();

  label->position.x = m_size.x / 2 - label_size.x / 2;
  label->position.y = m_size.y / 2 - label_size.y / 2;

  container->children.push_back(label);
  children.push_back(container);
}

void Button::update(std::vector<glm::mat4>& matrix_stack)
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

  if (m_input_manager->is_clicking(InputManager::MouseButton::Left))
  {
    const auto& mouse_position = m_input_manager->get_mouse_position();

    if (mouse_position.x > top_left.x && mouse_position.x < top_left.x + m_size.x && mouse_position.y > top_left.y &&
        mouse_position.y < top_left.y + m_size.y)
    {
      on_click();
    }
  }

  for (auto& child : children)
  {
    child->update(matrix_stack);
  }

  if (position.x != 0 && position.y != 0)
  {
    matrix_stack.pop_back();
  }
}

}  // namespace dl::ui
