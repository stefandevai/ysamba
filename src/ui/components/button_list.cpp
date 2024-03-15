#include "./button_list.hpp"

#include <spdlog/spdlog.h>

#include <entt/entity/entity.hpp>

#include "./button.hpp"
#include "./container.hpp"
#include "core/maths/vector.hpp"
#include "world/metadata.hpp"

namespace dl::ui
{
template <typename T>
ButtonList<T>::ButtonList(UIContext& context) : UIComponent(context)
{
}

template <typename T>
void ButtonList<T>::init()
{
  m_create_buttons();
}

template <typename T>
void ButtonList<T>::m_create_buttons()
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

  size = Vector2i{button_size.x, height};
}

template <typename T>
void ButtonList<T>::set_items(const ItemList<T>& items)
{
  m_items = items;
  has_initialized = false;
  dirty = true;
}

template <typename T>
void ButtonList<T>::set_on_select(const std::function<void(const T)>& on_select)
{
  this->on_select = on_select;
  has_initialized = false;
  dirty = true;
}

// Explicit instantiation of use cases
template class ButtonList<uint32_t>;
template class ButtonList<entt::entity>;
template class ButtonList<std::pair<entt::entity, entt::entity>>;
template class ButtonList<WorldMetadata>;
}  // namespace dl::ui
