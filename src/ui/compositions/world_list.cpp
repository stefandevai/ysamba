#include "./world_list.hpp"

#include <spdlog/spdlog.h>

#include <entt/core/hashed_string.hpp>

#include "core/maths/vector.hpp"
#include "ui/animation.hpp"
#include "ui/components/button_list.hpp"
#include "ui/components/scrollable_list.hpp"
#include "ui/components/window_frame.hpp"

namespace dl::ui
{
WorldList::WorldList(UIContext& context) : UIComponent(context)
{
  state = UIComponent::State::Hidden;

  m_window_frame = emplace<WindowFrame>();
  m_window_frame->size = Vector2i{300, 485};
  m_window_frame->x_alignment = XAlignement::Center;
  m_window_frame->y_alignment = YAlignement::Center;

  m_scrollable_list = m_window_frame->emplace<ScrollableList<WorldMetadata>>();
  m_scrollable_list->size = Vector2i{252, 437};
  m_scrollable_list->position = Vector3i{40, 40, 0};
  m_scrollable_list->list->line_spacing = 20;
}

void WorldList::update()
{
  using namespace entt::literals;

  if (!m_input_manager.is_context("list_selection"_hs))
  {
    return;
  }

  if (m_input_manager.poll_action("quit"_hs))
  {
    hide();
  }
}

void WorldList::show()
{
  using namespace entt::literals;

  m_input_manager.push_context("list_selection"_hs);
  m_scrollable_list->reset_scroll();
  animate<AnimationFadeIn>(0.3, Easing::OutQuart);
}

void WorldList::hide()
{
  m_input_manager.pop_context();
  animate<AnimationFadeOut>(0.3, Easing::OutQuart);
}

void WorldList::set_actions(const ItemList<WorldMetadata>& actions)
{
  dirty = true;
  m_scrollable_list->set_items(actions);
}

void WorldList::set_on_select(const std::function<void(const WorldMetadata&)>& on_select)
{
  m_scrollable_list->set_on_select(
      [this, on_select](const WorldMetadata& metadata)
      {
        // Hide component before selecting a world so that the input context is correctly popped
        hide();
        on_select(metadata);
      });
}

}  // namespace dl::ui
