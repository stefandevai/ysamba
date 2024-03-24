#include "./society_inventory.hpp"

#include <spdlog/spdlog.h>

#include "core/maths/vector.hpp"
#include "ui/animation.hpp"
#include "ui/components/label.hpp"
#include "ui/components/scrollable_list.hpp"
#include "ui/components/text_input.hpp"
#include "ui/components/window_frame.hpp"

namespace dl::ui
{
SocietyInventory::SocietyInventory(UIContext& context, const std::function<void(const int i)>& on_select)
    : UIComponent(context)
{
  state = UIComponent::State::Hidden;

  m_window_frame = emplace<WindowFrame>();
  m_window_frame->size = Vector2i{500, 400};
  m_window_frame->x_alignment = XAlignement::Center;
  m_window_frame->y_alignment = YAlignement::Center;

  m_items = m_window_frame->emplace<ScrollableList<uint32_t>>();
  m_items->title = "Items";
  m_items->size = Vector2i{452, 352};
  m_items->position = Vector3i{24, 24, 0};
  m_items->set_on_select(on_select);
}

void SocietyInventory::set_items(const ItemList<uint32_t>& items)
{
  m_items->set_items(items);
  dirty = true;
}

void SocietyInventory::show() { animate<AnimationFadeIn>(0.3, Easing::OutQuart); }

void SocietyInventory::hide() { animate<AnimationFadeOut>(0.3, Easing::OutQuart); }

}  // namespace dl::ui
