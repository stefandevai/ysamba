#include "./list.hpp"

#include "./container.hpp"
#include "./label.hpp"
#include "core/maths/vector.hpp"

namespace dl::ui
{
List::List() : UIComponent()
{
  const auto padding = Vector2i{25, 15};
  const auto width = 100;
  const auto line_spacing = 20;
  const auto max_height = 300;

  std::vector<std::string> keys = {
      "Item 1",
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

  auto container = std::make_shared<Container>(Vector2i{0, 0}, "#1b2420aa");
  container->position.x = 100;
  container->position.y = 100;

  auto first_label = std::make_shared<Label>(keys[0]);
  first_label->position.x = padding.x;
  first_label->position.y = padding.y + line_spacing / 2;
  container->children.push_back(first_label);

  const auto line_height = first_label->text.get_size().y;

  for (size_t i = 1; i < keys.size(); ++i)
  {
    auto label = std::make_shared<Label>(keys[i]);
    label->position.x = padding.x;
    label->position.y = i * line_height + padding.y + i * line_spacing + line_spacing / 2;
    container->children.push_back(label);
  }

  const auto height =
      std::min(static_cast<int>(keys.size() * (line_height + line_spacing)) + 2 * padding.y, max_height);

  container->set_size({width + 2 * padding.x, height});
  children.push_back(container);
}
}  // namespace dl::ui
