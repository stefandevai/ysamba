#pragma once

#include <spdlog/spdlog.h>

#include <entt/entity/entity.hpp>

#include "./button.hpp"
#include "./component.hpp"
#include "./container.hpp"
#include "./label.hpp"
#include "core/maths/vector.hpp"
#include "graphics/color.hpp"
#include "ui/types.hpp"
#include "world/metadata.hpp"

namespace dl::ui
{
class Button;

template <class T = uint32_t>
class ButtonList : public UIComponent
{
 public:
  int line_spacing = 0;
  Vector2i button_size = {200, 200};
  Color button_text_color = Color{0xFFFFFFFF};
  std::function<void(const T)> on_select;

  ButtonList(UIContext& context) : UIComponent(context) {}

  void init()
  {
    m_create_buttons();
  }

  void set_items(const ItemList<T>& items)
  {
    m_items = items;
    has_initialized = false;
    dirty = true;
  }

  void set_on_select(const std::function<void(const T)>& on_select)
  {
    this->on_select = on_select;
    has_initialized = false;
    dirty = true;
  }

 private:
  ItemList<T> m_items;

  void m_create_buttons()
  {
    children.clear();

    if (m_items.empty())
    {
      return;
    }

    const auto items_size = m_items.size();

    children.reserve(items_size);

    for (size_t i = 0; i < items_size; ++i)
    {
      const auto& item = m_items[i];

      auto button = emplace<Button>();
      button->text = item.second;
      button->label->text.color = button_text_color;
      button->size = Vector2i{button_size.x - 2 * margin.x, button_size.y};
      button->label_x_alignment = XAlignement::Left;
      button->label_y_alignment = YAlignement::Center;
      button->position.x = margin.x;
      button->position.y = i * (button_size.y + line_spacing) + margin.y + i;
      button->background_color = 0x00000000;

      if (on_select)
      {
        button->on_click = [this, &item]() { this->on_select(item.first); };
      }
    }

    const auto height = static_cast<int>(items_size * button_size.y + (items_size - 1) * line_spacing + 2 * margin.y);

    size = Vector2i{button_size.x, height};
  }
};

}  // namespace dl::ui
