#include "./action_menu.hpp"

#include <spdlog/spdlog.h>

#include "core/maths/vector.hpp"
#include "ui/animation.hpp"
#include "ui/components/scrollable_text_button_list.hpp"
#include "ui/components/window_frame.hpp"

namespace dl::ui
{
ActionMenu::ActionMenu(UIContext& context, Params params) : UIComponent(context, "ActionMenu")
{
  state = UIComponent::State::Hidden;
  size = Vector2i{300, 485};
  x_alignment = XAlignement::Center;
  y_alignment = YAlignement::Center;

  m_window_frame = emplace<WindowFrame>(WindowFrame::Params{
      .size = size,
  });

  m_list = m_window_frame->emplace<ScrollableTextButtonList<JobType>>(ScrollableTextButtonList<JobType>::Params{
      .items = std::move(params.actions),
      .on_left_click = std::move(params.on_select),
      .size = m_window_frame->get_safe_area_size(),
      .title = "Select Action",
      .enumerate = true,
      .enumeration_type = EnumerationType::Alphabetical,
  });

  const auto position_offset = m_window_frame->get_position_offset();
  m_list->position.x = position_offset.x;
  m_list->position.y = position_offset.y;
}

void ActionMenu::process_input()
{
  using namespace entt::literals;

  if (!m_input_manager.is_context("list_selection"_hs))
  {
    return;
  }

  if (m_input_manager.poll_action("close"_hs))
  {
    hide();
  }
}

void ActionMenu::show()
{
  using namespace entt::literals;

  m_input_manager.push_context("list_selection"_hs);
  m_list->scrollable->reset_scroll();
  animate<AnimationFadeIn>(0.3, Easing::OutQuart);
}

void ActionMenu::hide()
{
  m_input_manager.pop_context();
  animate<AnimationFadeOut>(0.3, Easing::OutQuart);
}

void ActionMenu::set_actions(const ItemList<JobType> actions)
{
  m_list->list->items = std::move(actions);
  m_list->list->create_buttons();
}

void ActionMenu::set_on_select(const std::function<void(JobType)> on_select)
{
  m_list->list->on_left_click = std::move(on_select);
}

}  // namespace dl::ui
