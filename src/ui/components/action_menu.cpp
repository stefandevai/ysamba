#include "./action_menu.hpp"

#include <spdlog/spdlog.h>

#include "./scrollable_list.hpp"
#include "core/maths/vector.hpp"

namespace dl::ui
{
ActionMenu::ActionMenu(const std::vector<std::string>& items, const std::function<void(const int i)>& on_select)
    : UIComponent()
{
  m_scrollable_list = std::make_shared<ScrollableList>(items, Vector2i{200, 300}, on_select);
  m_scrollable_list->x_alignment = XAlignement::Center;
  m_scrollable_list->y_alignment = YAlignement::Center;
  children.push_back(m_scrollable_list);
}

}  // namespace dl::ui
