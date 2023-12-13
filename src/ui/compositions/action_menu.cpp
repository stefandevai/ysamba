#include "./action_menu.hpp"

#include <spdlog/spdlog.h>

#include "core/maths/vector.hpp"
#include "ui/animation.hpp"
#include "ui/components/scrollable_list.hpp"

namespace dl::ui
{
ActionMenu::ActionMenu(const ItemList& items, const std::function<void(const uint32_t)>& on_select) : UIComponent()
{
  state = UIComponent::State::Hidden;
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

void ActionMenu::show() { animate<AnimationFadeIn>(this, 1.0, Easing::InOutCubic); }

void ActionMenu::hide() { animate<AnimationFadeOut>(this, 1.0, Easing::InOutCubic); }

}  // namespace dl::ui
