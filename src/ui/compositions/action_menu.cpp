#include "./action_menu.hpp"

#include <spdlog/spdlog.h>

#include "core/maths/vector.hpp"
#include "ui/animation.hpp"
#include "ui/components/scrollable_list.hpp"
#include "ui/components/window_frame.hpp"

namespace dl::ui
{
ActionMenu::ActionMenu(UIContext& context,
                       const ItemList<uint32_t>& items,
                       const std::function<void(const uint32_t)>& on_select)
    : UIComponent(context)
{
  state = UIComponent::State::Hidden;

  m_window_frame = emplace<WindowFrame>();
  m_window_frame->size = Vector2i{200, 300};
  m_window_frame->x_alignment = XAlignement::Center;
  m_window_frame->y_alignment = YAlignement::Center;

  m_scrollable_list = m_window_frame->emplace<ScrollableList<uint32_t>>();
  m_scrollable_list->size = Vector2i{152, 252};
  m_scrollable_list->position = Vector3i{24, 24, 0};
  m_scrollable_list->set_on_select(on_select);

  if (items.size() > 0)
  {
    m_scrollable_list->set_items(items);
  }
}

void ActionMenu::set_actions(const ItemList<uint32_t>& actions)
{
  dirty = true;
  m_scrollable_list->set_items(actions);
}

void ActionMenu::set_on_select(const std::function<void(const uint32_t)>& on_select)
{
  m_scrollable_list->set_on_select(on_select);
}

void ActionMenu::show()
{
  m_scrollable_list->reset_scroll();
  animate<AnimationFadeIn>(0.3, Easing::OutQuart);
}

void ActionMenu::hide()
{
  animate<AnimationFadeOut>(0.3, Easing::OutQuart);
}

}  // namespace dl::ui
