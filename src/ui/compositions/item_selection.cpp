#include "./item_selection.hpp"

#include <spdlog/spdlog.h>

#include "core/maths/vector.hpp"
#include "ui/animation.hpp"
#include "ui/components/label.hpp"
#include "ui/components/scrollable_list.hpp"
#include "ui/components/window_frame.hpp"

namespace dl::ui
{
ItemSelection::ItemSelection(UIContext& context, const std::function<void(const EntityPair&)>& on_select)
    : UIComponent(context)
{
  state = UIComponent::State::Hidden;

  m_window_frame = emplace<WindowFrame>();
  m_window_frame->size = Vector2i{300, 400};
  m_window_frame->x_alignment = XAlignement::Center;
  m_window_frame->y_alignment = YAlignement::Center;

  m_items = m_window_frame->emplace<ScrollableList<EntityPair>>();
  m_items->title = "Select Item";
  m_items->size = Vector2i{252, 352};
  m_items->position = Vector3i{24, 24, 0};
  m_items->set_on_select(on_select);
}

void ItemSelection::set_items(const ItemList<EntityPair>& items)
{
  m_items->set_items(items);
  dirty = true;
}

void ItemSelection::show()
{
  m_items->reset_scroll();
  animate<AnimationFadeIn>(0.3, Easing::OutQuart);
}

void ItemSelection::hide()
{
  animate<AnimationFadeOut>(0.3, Easing::OutQuart);
}

}  // namespace dl::ui
