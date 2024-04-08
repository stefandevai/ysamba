#include "./item_selection.hpp"

#include <spdlog/spdlog.h>

#include "core/maths/vector.hpp"
#include "ui/animation.hpp"
#include "ui/components/label.hpp"
#include "ui/components/scrollable_text_button_list.hpp"
#include "ui/components/window_frame.hpp"

namespace dl::ui
{
ItemSelection::ItemSelection(UIContext& context, const std::function<void(const EntityPair&)>& on_select)
    : UIComponent(context, "ItemSelection")
{
  state = UIComponent::State::Hidden;
  size = Vector2i{300, 400};
  x_alignment = XAlignement::Center;
  y_alignment = YAlignement::Center;

  m_window_frame = emplace<WindowFrame>(WindowFrame::Params{
      .size = size,
  });

  m_items = m_window_frame->emplace<ScrollableTextButtonList<EntityPair>>(ScrollableTextButtonList<EntityPair>::Params{
      .size = m_window_frame->get_safe_area_size(),
      .on_left_click = on_select,
      .title = "Select Item",
  });

  const auto position_offset = m_window_frame->get_position_offset();
  m_items->position.x = position_offset.x;
  m_items->position.y = position_offset.y;
}

void ItemSelection::set_items(const ItemList<EntityPair>& items)
{
  m_items->list->items = items;
  m_items->list->create_buttons();
}

void ItemSelection::show()
{
  m_items->scrollable->reset_scroll();
  animate<AnimationFadeIn>(0.3, Easing::OutQuart);
}

void ItemSelection::hide()
{
  animate<AnimationFadeOut>(0.3, Easing::OutQuart);
}

}  // namespace dl::ui
