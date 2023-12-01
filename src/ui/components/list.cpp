#include "./list.hpp"

#include <spdlog/spdlog.h>

#include "./container.hpp"
#include "./label.hpp"
#include "core/display.hpp"
#include "core/maths/vector.hpp"
#include "graphics/renderer.hpp"

namespace dl::ui
{
List::List() : UIComponent()
{
  const auto padding = Vector2i{0, 0};
  const auto line_spacing = 20;
  /* const auto max_height = 300; */

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

  auto first_label = std::make_shared<Label>(keys[0]);
  first_label->position.x = padding.x;
  first_label->position.y = padding.y + line_spacing / 2;
  children.push_back(first_label);

  const auto& first_label_size = first_label->text.get_size();
  const auto line_height = first_label_size.y;

  auto width = first_label_size.x;

  for (size_t i = 1; i < keys.size(); ++i)
  {
    auto label = std::make_shared<Label>(keys[i]);
    label->position.x = padding.x;
    label->position.y = i * (line_height + line_spacing) + padding.y + line_spacing / 2;
    children.push_back(label);

    const auto& line_size = label->text.get_size();
    width = std::max(width, line_size.x);
  }

  const auto height = static_cast<int>(keys.size() * (line_height + line_spacing)) + 2 * padding.y;

  size = {width + 2 * padding.x, height};
}

}  // namespace dl::ui
