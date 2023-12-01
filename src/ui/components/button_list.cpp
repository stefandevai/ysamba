#include "./button_list.hpp"

#include <spdlog/spdlog.h>

#include "./button.hpp"
#include "./container.hpp"
#include "core/display.hpp"
#include "core/maths/vector.hpp"
#include "graphics/renderer.hpp"

namespace dl::ui
{
ButtonList::ButtonList(const std::vector<std::string>& items,
                       const Vector2i& button_size,
                       std::function<void(const int)> on_select,
                       const ListStyle& style)
    : UIComponent(), style(style), on_select(on_select)
{
  if (items.size() < 1)
  {
    return;
  }

  const auto items_size = items.size();
  children.reserve(items_size);

  for (size_t i = 0; i < items_size; ++i)
  {
    auto button = std::make_shared<Button>(items[i],
                                           Vector2i{button_size.x - 2 * style.margin.x, button_size.y},
                                           XAlignement::Center,
                                           YAlignement::Center);

    if (on_select)
    {
      button->on_click = [this, i]() { this->on_select(i); };
    }

    button->position.x = style.margin.x;
    button->position.y = i * (button_size.y + style.line_spacing) + style.margin.y;

    children.push_back(button);
  }

  const auto height =
      static_cast<int>(items_size * button_size.y + (items_size - 1) * style.line_spacing + 2 * style.margin.y);

  size = {button_size.x, height};
}

}  // namespace dl::ui
