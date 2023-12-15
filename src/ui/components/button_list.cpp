#include "./button_list.hpp"

#include <spdlog/spdlog.h>

#include "./button.hpp"
#include "./container.hpp"
#include "core/asset_manager.hpp"
#include "core/display.hpp"
#include "core/maths/vector.hpp"

namespace dl::ui
{
ButtonList::ButtonList(UIContext& context) : UIComponent(context) {}

void ButtonList::init() { m_create_buttons(); }

void ButtonList::m_create_buttons()
{
  if (m_items.empty())
  {
    return;
  }

  const auto items_size = m_items.size();

  children.clear();
  children.reserve(items_size);
  spdlog::debug("CREATING BUTTONS {}", items_size);

  for (size_t i = 0; i < items_size; ++i)
  {
    const auto& item = m_items[i];

    auto button = emplace<Button>();
    button->text = item.second;
    button->size = Vector2i{button_size.x - 2 * margin.x, button_size.y};
    button->label_x_alignment = XAlignement::Left;
    button->label_y_alignment = YAlignement::Center;
    button->position.x = margin.x;
    button->position.y = i * (button_size.y + line_spacing) + margin.y;
    button->background_color = 0x00000000;

    if (on_select)
    {
      button->on_click = [this, &item]() { this->on_select(item.first); };
    }
  }

  const auto height = static_cast<int>(items_size * button_size.y + (items_size - 1) * line_spacing + 2 * margin.y);

  size = {button_size.x, height};
}

void ButtonList::set_items(const ItemList& items)
{
  spdlog::debug("SETTING ITEMS");
  for (auto& item : items)
  {
    spdlog::debug("{} {}", item.first, item.second);
  }
  spdlog::debug("");

  m_items = items;
  has_initialized = false;
}

void ButtonList::set_on_select(const std::function<void(const uint32_t)>& on_select)
{
  this->on_select = on_select;
  has_initialized = false;
}

}  // namespace dl::ui
