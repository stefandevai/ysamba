#include "./inventory.hpp"

#include <spdlog/spdlog.h>

#include "core/maths/vector.hpp"
#include "ui/components/scrollable_list.hpp"

namespace dl::ui
{
Inventory::Inventory(UIContext& context, const std::function<void(const int i)>& on_select) : UIComponent(context)
{
  state = UIComponent::State::Hidden;
  m_scrollable_list = emplace<ScrollableList>();
  m_scrollable_list->size = Vector2i{500, 400};
  m_scrollable_list->x_alignment = XAlignement::Center;
  m_scrollable_list->y_alignment = YAlignement::Center;
  m_scrollable_list->set_items({});
  m_scrollable_list->set_on_select(on_select);
}

void Inventory::set_items(const ItemList& items)
{
  dirty = true;
  m_scrollable_list->set_items(items);
}

}  // namespace dl::ui
