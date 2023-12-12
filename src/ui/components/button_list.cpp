#include "./button_list.hpp"

#include <spdlog/spdlog.h>

#include "./button.hpp"
#include "./container.hpp"
#include "core/display.hpp"
#include "core/maths/vector.hpp"

namespace dl::ui
{
ButtonList::ButtonList(const ItemList& items, const std::function<void(const uint32_t)>& on_select)
    : UIComponent(), on_select(on_select), m_items(items)
{
}

void ButtonList::init()
{
  if (m_items.size() < 1)
  {
    return;
  }

  const auto items_size = m_items.size();
  children.reserve(items_size);
  m_buttons.reserve(items_size);

  for (size_t i = 0; i < items_size; ++i)
  {
    const auto& item = m_items[i];

    auto button = emplace<Button>(
        item.second, Vector2i{button_size.x - 2 * margin.x, button_size.y}, XAlignement::Center, YAlignement::Center);
    m_buttons.push_back(button);

    if (on_select)
    {
      button->on_click = [this, &item]() { this->on_select(item.first); };
    }

    button->position.x = margin.x;
    button->position.y = i * (button_size.y + line_spacing) + margin.y;
  }

  const auto height = static_cast<int>(items_size * button_size.y + (items_size - 1) * line_spacing + 2 * margin.y);

  size = {button_size.x, height};
}

void ButtonList::set_on_select(const std::function<void(const uint32_t)>& on_select) { this->on_select = on_select; }

}  // namespace dl::ui
