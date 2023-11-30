#include "./button.hpp"

#include <spdlog/spdlog.h>

#include "./container.hpp"
#include "./label.hpp"
#include "graphics/batch.hpp"

namespace dl::ui
{
Button::Button(const std::string& text, const Vector3i& position, const Vector2i& size) : UIComponent()
{
  this->position = position;
  this->size = size;

  auto container = std::make_shared<Container>(size, "#33aa88aa");
  auto label = std::make_shared<Label>(text);
  const auto& label_size = label->text.get_size();

  label->position.x = size.x / 2 - label_size.x / 2;
  label->position.y = size.y / 2 - label_size.y / 2;

  container->children.push_back(label);
  children.push_back(container);
}

void Button::update(std::vector<glm::mat4>& matrix_stack)
{
  if (m_input_manager->is_clicking(InputManager::MouseButton::Left))
  {
    const auto& matrix = matrix_stack.back();
    const auto top_left = matrix * glm::vec4(0.f, 0.f, 1.f, 1.f);
    const auto& mouse_position = m_input_manager->get_mouse_position();

    if (mouse_position.x > top_left.x && mouse_position.x < top_left.x + size.x && mouse_position.y > top_left.y &&
        mouse_position.y < top_left.y + size.y)
    {
      on_click();
    }
  }
}

}  // namespace dl::ui
