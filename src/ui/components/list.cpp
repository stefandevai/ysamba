#include "./list.hpp"

#include <spdlog/spdlog.h>

#include "./container.hpp"
#include "./label.hpp"
#include "core/display.hpp"
#include "core/maths/vector.hpp"

namespace dl::ui
{
List::List(UIContext& context, const std::vector<std::string>& items) : UIComponent(context), m_items(items) {}

void List::init()
{
  if (m_items.size() < 1)
  {
    return;
  }

  children.reserve(m_items.size());

  auto first_label = emplace<Label>(m_items[0]);
  first_label->position.x = margin.x;
  first_label->position.y = margin.y + line_spacing / 2;

  const auto& first_label_size = first_label->text.get_size();
  const auto line_height = first_label_size.y;

  auto width = first_label_size.x;

  for (size_t i = 1; i < m_items.size(); ++i)
  {
    auto label = emplace<Label>(m_items[i]);
    label->position.x = margin.x;
    label->position.y = i * (line_height + line_spacing) + margin.y + line_spacing / 2;

    const auto& line_size = label->text.get_size();
    width = std::max(width, line_size.x);
  }

  const auto height = static_cast<int>(m_items.size() * (line_height + line_spacing)) + 2 * margin.y;

  size = {width + 2 * margin.x, height};
}

}  // namespace dl::ui
