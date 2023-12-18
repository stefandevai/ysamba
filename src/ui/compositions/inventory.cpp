#include "./inventory.hpp"

#include <spdlog/spdlog.h>

#include "core/maths/vector.hpp"
#include "ui/animation.hpp"
#include "ui/components/label.hpp"
#include "ui/components/scrollable_list.hpp"
#include "ui/components/text_input.hpp"
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

  m_weared_items = m_window_frame->emplace<ScrollableList<uint32_t>>();
  m_weared_items->title = "Weared Items";
  m_weared_items->size = Vector2i{452, 352};
  m_weared_items->position = Vector3i{24, 24, 0};
  m_weared_items->set_on_select(on_select);

  m_carried_items = m_window_frame->emplace<ScrollableList<uint32_t>>();
  m_carried_items->title = "Carried Items";
  m_carried_items->size = Vector2i{452, 352};
  m_carried_items->position = Vector3i{224, 24, 0};
  m_carried_items->set_on_select(on_select);
}

void Inventory::set_weared_items(const ItemList<uint32_t>& items)
{
  m_weared_items->set_items(items);
  dirty = true;
}

void Inventory::set_carried_items(const ItemList<uint32_t>& items)
{
  m_carried_items->set_items(items);
  dirty = true;
}

void Inventory::show() { animate<AnimationFadeIn>(0.3, Easing::OutQuart); }

void Inventory::hide() { animate<AnimationFadeOut>(0.3, Easing::OutQuart); }

}  // namespace dl::ui
