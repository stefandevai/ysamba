#include "./inventory.hpp"

#include <spdlog/spdlog.h>

#include <entt/core/hashed_string.hpp>

#include "core/maths/vector.hpp"
#include "ui/animation.hpp"
#include "ui/components/label.hpp"
#include "ui/components/scrollable_list.hpp"
#include "ui/components/text_input.hpp"
#include "ui/components/window_frame.hpp"
#include "ui/ui_manager.hpp"

namespace dl::ui
{
Inventory::Inventory(UIContext& context, const std::function<void(entt::entity)>& on_select) : UIComponent(context)
{
  state = UIComponent::State::Hidden;

  m_window_frame = emplace<WindowFrame>();
  m_window_frame->size = Vector2i{500, 400};
  m_window_frame->x_alignment = XAlignement::Center;
  m_window_frame->y_alignment = YAlignement::Center;

  m_weared_items = m_window_frame->emplace<ScrollableList<entt::entity>>();
  m_weared_items->title = "Weared Items";
  m_weared_items->size = Vector2i{452, 352};
  m_weared_items->position = Vector3i{24, 24, 0};
  m_weared_items->set_on_select(on_select);

  m_carried_items = m_window_frame->emplace<ScrollableList<entt::entity>>();
  m_carried_items->title = "Carried Items";
  m_carried_items->size = Vector2i{452, 352};
  m_carried_items->position = Vector3i{224, 24, 0};
  m_carried_items->set_on_select(on_select);
}

void Inventory::process_input()
{
  using namespace entt::literals;

  if (!m_input_manager.is_context("inventory"_hs))
  {
    return;
  }

  if (m_input_manager.poll_action("close_inventory"_hs))
  {
    hide();
  }
}

void Inventory::set_weared_items(const ItemList<entt::entity>& items)
{
  m_weared_items->set_items(items);
  dirty = true;
}

void Inventory::set_carried_items(const ItemList<entt::entity>& items)
{
  m_carried_items->set_items(items);
  dirty = true;
}

void Inventory::show()
{
  using namespace entt::literals;

  m_input_manager.push_context("inventory"_hs);
  m_context.ui_manager->bring_to_front(this);
  m_context.focused_stack->push_back(this);
  animate<AnimationFadeIn>(0.3, Easing::OutQuart);
}

void Inventory::hide()
{
  animate<AnimationFadeOut>(0.3, Easing::OutQuart);
  m_context.focused_stack->pop_back();
  m_input_manager.pop_context();
}

}  // namespace dl::ui
