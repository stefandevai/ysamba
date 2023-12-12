#include "./inventory.hpp"

#include <spdlog/spdlog.h>

#include "./scrollable_list.hpp"
#include "core/maths/vector.hpp"

namespace dl::ui
{
Inventory::Inventory(const std::function<void(const int i)>& on_select) : UIComponent()
{
  m_scrollable_list = std::make_shared<ScrollableList>(Vector2i{500, 400}, on_select);
  m_scrollable_list->x_alignment = XAlignement::Center;
  m_scrollable_list->y_alignment = YAlignement::Center;
  children.push_back(m_scrollable_list);
}

void Inventory::set_items(const ItemList& items)
{
  dirty = true;
  m_scrollable_list->set_items(items);
}

}  // namespace dl::ui
