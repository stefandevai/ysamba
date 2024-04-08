#include "./inventory.hpp"

#include <spdlog/spdlog.h>

#include <entt/core/hashed_string.hpp>

#include "core/maths/vector.hpp"
#include "ui/animation.hpp"
#include "ui/components/label.hpp"
#include "ui/components/text_button_list.hpp"
#include "ui/components/text_input.hpp"
#include "ui/components/window_frame.hpp"
#include "ui/ui_manager.hpp"

namespace dl::ui
{
Inventory::Inventory(UIContext& context, const std::function<void(entt::entity)>& on_select)
    : UIComponent(context, "Inventory")
{
  state = UIComponent::State::Hidden;
  size = Vector2i{500, 400};
  x_alignment = XAlignement::Center;
  y_alignment = YAlignement::Center;

  m_window_frame = emplace<WindowFrame>(WindowFrame::Params{
      .size = size,
  });

  const auto safe_area_size = m_window_frame->get_safe_area_size();
  const auto position_offset = m_window_frame->get_position_offset();

  m_weared_items = m_window_frame->emplace<TextButtonList<entt::entity>>(TextButtonList<entt::entity>::Params{
      .size = Vector2i{safe_area_size.x / 2, safe_area_size.y},
      .on_left_click = on_select,
      .title = "Weared Items",
  });

  m_weared_items->position.x = position_offset.x;
  m_weared_items->position.y = position_offset.y;

  m_carried_items = m_window_frame->emplace<TextButtonList<entt::entity>>(TextButtonList<entt::entity>::Params{
      .size = Vector2i{safe_area_size.x / 2, safe_area_size.y},
      .on_left_click = on_select,
      .title = "Carried Items",
  });

  m_carried_items->position.x = position_offset.x + safe_area_size.x / 2;
  m_carried_items->position.y = position_offset.y;
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
  m_weared_items->items = items;
  m_weared_items->create_buttons();
}

void Inventory::set_carried_items(const ItemList<entt::entity>& items)
{
  m_carried_items->items = items;
  m_carried_items->create_buttons();
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
