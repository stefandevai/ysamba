#include "./inventory.hpp"

#include <spdlog/spdlog.h>

#include "core/maths/vector.hpp"
#include "ui/animation.hpp"
#include "ui/components/label.hpp"
#include "ui/components/scrollable_list.hpp"
#include "ui/components/window_frame.hpp"

namespace dl::ui
{
Inventory::Inventory(UIContext& context, const std::function<void(const int i)>& on_select) : UIComponent(context)
{
  state = UIComponent::State::Hidden;

  m_window_frame = emplace<WindowFrame>();
  m_window_frame->size = Vector2i{500, 400};
  m_window_frame->x_alignment = XAlignement::Center;
  m_window_frame->y_alignment = YAlignement::Center;

  m_scrollable_list = m_window_frame->emplace<ScrollableList>();
  m_scrollable_list->size = Vector2i{452, 352};
  m_scrollable_list->position = Vector3i{24, 24, 0};
  m_scrollable_list->set_on_select(on_select);
}

void Inventory::set_items(const ItemList& items)
{
  m_scrollable_list->set_items(items);
  dirty = true;
}

void Inventory::show()
{
  m_scrollable_list->reset_scroll();
  animate<AnimationFadeIn>(0.3, Easing::OutQuart);
}

void Inventory::hide() { animate<AnimationFadeOut>(0.3, Easing::OutQuart); }

}  // namespace dl::ui
