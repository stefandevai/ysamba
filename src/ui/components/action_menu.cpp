#include "./action_menu.hpp"

#include <spdlog/spdlog.h>

#include "./scrollable_list.hpp"
#include "core/maths/vector.hpp"

namespace dl::ui
{
ActionMenu::ActionMenu(const ItemList& items, const std::function<void(const uint32_t)>& on_select) : UIComponent()
{
  visible = false;
  m_scrollable_list = emplace<ScrollableList>();
  m_scrollable_list->size = Vector2i{200, 300};
  m_scrollable_list->x_alignment = XAlignement::Center;
  m_scrollable_list->y_alignment = YAlignement::Center;
  m_scrollable_list->set_items(items);
  m_scrollable_list->set_on_select(on_select);
}

void ActionMenu::set_actions(const ItemList& actions)
{
  dirty = true;
  m_scrollable_list->set_items(actions);
}

void ActionMenu::set_on_select(const std::function<void(const uint32_t)>& on_select)
{
  m_scrollable_list->set_on_select(on_select);
}

}  // namespace dl::ui
