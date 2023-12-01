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
  const auto padding = Vector2i{0, 0};
  const auto width = 200;
  const auto line_spacing = 20;
  const auto max_height = 300;

  std::vector<std::string> keys = {
      "Item 1aaaaaaaaaaaaaaaaaaaaaaaaaaa",
      "Item 2",
      "Item 3",
      "Item 4",
      "Item 5",
      "Item 1",
      "Item 2",
      "Item 3",
      "Item 4",
      "Item 5",
  };

  if (keys.size() < 1)
  {
    return;
  }

  position.x = 100;
  position.y = 100;

  /* auto container = std::make_shared<Container>(Vector2i{0, 0}, "#1b2420aa"); */

  auto first_label = std::make_shared<Label>(keys[0]);
  first_label->position.x = padding.x;
  first_label->position.y = padding.y + line_spacing / 2;
  /* container->children.push_back(first_label); */
  children.push_back(first_label);

  const auto line_height = first_label->text.get_size().y;

  for (size_t i = 1; i < keys.size(); ++i)
  {
    auto label = std::make_shared<Label>(keys[i]);
    label->position.x = padding.x;
    label->position.y = i * line_height + padding.y + i * line_spacing + line_spacing / 2;
    /* container->children.push_back(label); */
    children.push_back(label);
  }

  const auto height =
      std::min(static_cast<int>(keys.size() * (line_height + line_spacing)) + 2 * padding.y, max_height);

  size = {width + 2 * padding.x, height};
  /* container->set_size({width + 2 * padding.x, height}); */
  /* children.push_back(container); */
}

void Scrollable::update(std::vector<glm::mat4>& matrix_stack)
{
  const auto& matrix = matrix_stack.back();

  if (m_input_manager->is_scrolling_y())
  {
    const auto top_left = matrix * glm::vec4(0.f, 0.f, 1.f, 1.f);
    const auto& mouse_position = m_input_manager->get_mouse_position();

    if (mouse_position.x > top_left.x && mouse_position.x < top_left.x + size.x && mouse_position.y > top_left.y &&
        mouse_position.y < top_left.y + size.y)
    {
      m_scroll_y += m_input_manager->get_scroll().y * 4;
    }
  }

  matrix_stack.pop_back();
  const auto translate = glm::translate(matrix, glm::vec3(0, m_scroll_y, 0));
  matrix_stack.push_back(translate);
}

void Scrollable::render(Renderer& renderer, const std::string& layer)
{
  (void)layer;

  const auto& window_size = Display::get_window_size();
  renderer.get_layer("ui-2")->add_scissor(
      {absolute_position.x, window_size.y - absolute_position.y - size.y, size.x, size.y});

  for (auto& child : children)
  {
    child->render(renderer, "ui-2");
  }
}

}  // namespace dl::ui
