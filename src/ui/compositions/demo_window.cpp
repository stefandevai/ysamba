#include "./demo_window.hpp"

#include <spdlog/spdlog.h>

#include "core/maths/vector.hpp"
#include "ui/animation.hpp"
#include "ui/components/container.hpp"
#include "ui/components/flex.hpp"
#include "ui/components/scrollable_text_button_list.hpp"
#include "ui/components/window_frame.hpp"

namespace dl::ui
{
DemoWindow::DemoWindow(UIContext& context) : UIComponent(context, "DemoWindow")
{
  state = UIComponent::State::Hidden;
  size = Vector2i{700, 685};
  x_alignment = XAlignement::Center;
  y_alignment = YAlignement::Center;

  m_window_frame = emplace<WindowFrame>(WindowFrame::Params{
      .size = size,
  });

  auto safe_area = m_window_frame->get_safe_area_size();

  // auto flex1 = m_window_frame->emplace<Flex>(Flex::Params{
  //     .size = safe_area,
  //     .vertical_placement = FlexPlacement::Center,
  //     .horizontal_placement = FlexPlacement::SpaceBetween,
  // });
  //
  // flex1->emplace<Container>(Container::Params{.size = {100, 100}, .color = 0xFF8844FF});
  // flex1->emplace<Container>(Container::Params{.size = {30, 130}, .color = 0x6688FFFF});
  // flex1->emplace<Container>(Container::Params{.size = {200, 80}, .color = 0x99FF44FF});

  auto flex2 = m_window_frame->emplace<Flex>(Flex::Params{
      .size = safe_area,
      .direction = FlexDirection::Column,
      // .vertical_placement = FlexPlacement::SpaceEvenly,
      // .horizontal_placement = FlexPlacement::SpaceBetween,
  });
  flex2->emplace<Container>(Container::Params{.size = {100, 100}, .color = 0xFF8844FF});
  flex2->emplace<Container>(Container::Params{.size = {30, 130}, .color = 0x6688FFFF});
  flex2->emplace<Container>(Container::Params{.size = {200, 80}, .color = 0x99FF44FF});

  const auto position_offset = m_window_frame->get_position_offset();
  // flex1->position.x = position_offset.x;
  // flex1->position.y = position_offset.y;
  flex2->position.x = position_offset.x;
  flex2->position.y = position_offset.y;
}

void DemoWindow::process_input()
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

void DemoWindow::show()
{
  using namespace entt::literals;

  m_input_manager.push_context("list_selection"_hs);
  // m_list->scrollable->reset_scroll();
  animate<AnimationFadeIn>(0.3, Easing::OutQuart);
}

void DemoWindow::hide()
{
  m_input_manager.pop_context();
  animate<AnimationFadeOut>(0.3, Easing::OutQuart);
}
}  // namespace dl::ui
