#include "./list.hpp"

#include <spdlog/spdlog.h>

#include "./container.hpp"
#include "./label.hpp"
#include "core/display.hpp"
#include "core/maths/vector.hpp"
#include "graphics/renderer.hpp"

namespace dl::ui
{
List::List(const std::vector<std::string>& items, const ListStyle& style) : UIComponent(), style(style)
{
  if (items.size() < 1)
  {
    return;
  }

  children.reserve(items.size());

  auto first_label = emplace<Label>(items[0]);
  first_label->position.x = style.margin.x;
  first_label->position.y = style.margin.y + style.line_spacing / 2;

  const auto& first_label_size = first_label->text.get_size();
  const auto line_height = first_label_size.y;

  auto width = first_label_size.x;

  for (size_t i = 1; i < items.size(); ++i)
  {
    auto label = emplace<Label>(items[i]);
    label->position.x = style.margin.x;
    label->position.y = i * (line_height + style.line_spacing) + style.margin.y + style.line_spacing / 2;

    const auto& line_size = label->text.get_size();
    width = std::max(width, line_size.x);
  }

  const auto height = static_cast<int>(items.size() * (line_height + style.line_spacing)) + 2 * style.margin.y;

  size = {width + 2 * style.margin.x, height};
}

}  // namespace dl::ui
