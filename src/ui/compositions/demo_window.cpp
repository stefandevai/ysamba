#include "./demo_window.hpp"

#include <spdlog/spdlog.h>

#include "core/maths/vector.hpp"
#include "ui/animation.hpp"
#include "ui/components/scrollable_text_button_list.hpp"
#include "ui/components/window_frame.hpp"
#include "ui/components/flex.hpp"
#include "ui/components/container.hpp"

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

  auto flex = m_window_frame->emplace<Flex>(Flex::Params{});
  flex->emplace<Container>(Container::Params{.size = {100, 100}, .color = 0xFF8844FF});
  flex->emplace<Container>(Container::Params{.size = {200, 100}, .color = 0x99FF44FF});


  // m_list = m_window_frame->emplace<ScrollableTextButtonList<uint32_t>>(ScrollableTextButtonList<uint32_t>::Params{
  //     .items = {{0, "Item0"}},
  //     .size = m_window_frame->get_safe_area_size(),
  //     .title = "Select Action",
  //     .enumerate = true,
  //     .enumeration_type = EnumerationType::Alphabetical,
  // });

  const auto position_offset = m_window_frame->get_position_offset();
  flex->position.x = position_offset.x;
  flex->position.y = position_offset.y;
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
