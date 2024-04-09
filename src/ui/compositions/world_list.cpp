#include "./world_list.hpp"

#include <spdlog/spdlog.h>

#include <entt/core/hashed_string.hpp>

#include "core/maths/vector.hpp"
#include "ui/animation.hpp"
#include "ui/components/scrollable_text_button_list.hpp"
#include "ui/components/window_frame.hpp"

namespace dl::ui
{
WorldList::WorldList(UIContext& context, Params params) : UIComponent(context, "WorldList")
{
  state = UIComponent::State::Hidden;
  size = Vector2i{300, 485};
  x_alignment = XAlignement::Center;
  y_alignment = YAlignement::Center;

  m_window_frame = emplace<WindowFrame>(WindowFrame::Params{
      .size = size,
  });

  m_list = m_window_frame->emplace<ScrollableTextButtonList<WorldMetadata>>(
      ScrollableTextButtonList<WorldMetadata>::Params{
          .items = std::move(params.items),
          .on_left_click = std::move(params.on_select),
          .size = m_window_frame->get_safe_area_size(),
          .line_spacing = 0,
          .button_size = {0, 48},
          .title = "Select World",
          .enumerate = true,
          .enumeration_type = EnumerationType::Alphabetical,
      });

  const auto position_offset = m_window_frame->get_position_offset();
  m_list->position.x = position_offset.x;
  m_list->position.y = position_offset.y;
}

void WorldList::process_input()
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

void WorldList::show()
{
  using namespace entt::literals;

  m_input_manager.push_context("list_selection"_hs);
  m_list->scrollable->reset_scroll();
  animate<AnimationFadeIn>(0.3, Easing::OutQuart);
}

void WorldList::hide()
{
  m_input_manager.pop_context();
  animate<AnimationFadeOut>(0.3, Easing::OutQuart);
}

void WorldList::set_items(const ItemList<WorldMetadata> items)
{
  m_list->list->items = std::move(items);
  m_list->list->create_buttons();
}

void WorldList::set_on_select(const std::function<void(const WorldMetadata&)>& on_select)
{
  m_list->list->on_left_click = [this, on_select](const WorldMetadata& metadata)
  {
    // Hide component before selecting a world so that the input context is correctly popped
    hide();
    on_select(metadata);
  };
}

}  // namespace dl::ui
