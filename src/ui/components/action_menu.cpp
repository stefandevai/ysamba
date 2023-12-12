#include "./action_menu.hpp"

#include <spdlog/spdlog.h>

#include "./scrollable_list.hpp"
#include "core/maths/vector.hpp"

namespace dl::ui
{
ActionMenu::ActionMenu(const ItemList& items, const std::function<void(const uint32_t)>& on_select) : UIComponent()
{
  visible = false;
  m_scrollable_list = std::make_shared<ScrollableList>(items, Vector2i{200, 300}, on_select);
  m_scrollable_list->x_alignment = XAlignement::Center;
  m_scrollable_list->y_alignment = YAlignement::Center;
  children.push_back(m_scrollable_list);
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
