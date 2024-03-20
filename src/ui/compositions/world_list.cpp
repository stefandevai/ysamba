#include "./world_list.hpp"

#include <spdlog/spdlog.h>

#include "core/maths/vector.hpp"
#include "ui/animation.hpp"
#include "ui/components/scrollable_list.hpp"
#include "ui/components/window_frame.hpp"
#include "ui/components/button_list.hpp"

namespace dl::ui
{
WorldList::WorldList(UIContext& context) : UIComponent(context)
{
  state = UIComponent::State::Hidden;

  m_window_frame = emplace<WindowFrame>();
  m_window_frame->size = Vector2i{200, 300};
  m_window_frame->x_alignment = XAlignement::Center;
  m_window_frame->y_alignment = YAlignement::Center;

  m_scrollable_list = m_window_frame->emplace<ScrollableList<WorldMetadata>>();
  m_scrollable_list->size = Vector2i{152, 252};
  m_scrollable_list->position = Vector3i{24, 24, 0};
  m_scrollable_list->list->line_spacing = 15;
}

void WorldList::set_actions(const ItemList<WorldMetadata>& actions)
{
  dirty = true;
  m_scrollable_list->set_items(actions);
}

void WorldList::set_on_select(const std::function<void(const WorldMetadata&)>& on_select)
{
  m_scrollable_list->set_on_select(on_select);
}

void WorldList::show()
{
  m_scrollable_list->reset_scroll();
  animate<AnimationFadeIn>(0.3, Easing::OutQuart);
}

void WorldList::hide() { animate<AnimationFadeOut>(0.3, Easing::OutQuart); }

}  // namespace dl::ui
