#include "./action_menu.hpp"

#include <spdlog/spdlog.h>

#include "core/maths/vector.hpp"
#include "ui/animation.hpp"
#include "ui/components/scrollable_text_button_list.hpp"
#include "ui/components/window_frame.hpp"

namespace dl::ui
{
ActionMenu::ActionMenu(UIContext& context,
                       const ItemList<JobType>& items,
                       const std::function<void(const JobType)>& on_select)
    : UIComponent(context, "ActionMenu")
{
  state = UIComponent::State::Hidden;
  size = Vector2i{300, 485};
  x_alignment = XAlignement::Center;
  y_alignment = YAlignement::Center;

  m_window_frame = emplace<WindowFrame>(WindowFrame::Params{
      .size = size,
  });

  m_list = m_window_frame->emplace<ScrollableTextButtonList<JobType>>(ScrollableTextButtonList<JobType>::Params{
      .size = m_window_frame->get_safe_area_size(),
      .on_left_click = on_select,
      .items = items,
      .title = "Select Action",
  });

  const auto position_offset = m_window_frame->get_position_offset();
  m_list->position.x = position_offset.x;
  m_list->position.y = position_offset.y;
}

void ActionMenu::set_actions(const ItemList<JobType>& actions)
{
  m_list->list->items = actions;
  m_list->list->create_buttons();
}

void ActionMenu::set_on_select(const std::function<void(const JobType)>& on_select)
{
  m_list->list->on_left_click = on_select;
}

void ActionMenu::show()
{
  m_list->scrollable->reset_scroll();
  animate<AnimationFadeIn>(0.3, Easing::OutQuart);
}

void ActionMenu::hide()
{
  animate<AnimationFadeOut>(0.3, Easing::OutQuart);
}

}  // namespace dl::ui
