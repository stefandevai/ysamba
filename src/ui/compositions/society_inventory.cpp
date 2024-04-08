#include "./society_inventory.hpp"

#include <spdlog/spdlog.h>

#include <entt/core/hashed_string.hpp>

#include "core/maths/vector.hpp"
#include "ui/animation.hpp"
#include "ui/components/scrollable_text_button_list.hpp"
#include "ui/components/window_frame.hpp"
#include "ui/ui_manager.hpp"

namespace dl::ui
{
SocietyInventory::SocietyInventory(UIContext& context, const std::function<void(entt::entity)>& on_select)
    : UIComponent(context, "SocietyInventory")
{
  state = UIComponent::State::Hidden;
  size = Vector2i{500, 400};
  x_alignment = XAlignement::Center;
  y_alignment = YAlignement::Center;

  m_window_frame = emplace<WindowFrame>(WindowFrame::Params{
      .size = size,
  });

  m_items = m_window_frame->emplace<ScrollableTextButtonList<entt::entity>>(ScrollableTextButtonList<entt::entity>::Params{
      .size = m_window_frame->get_safe_area_size(),
      .on_left_click = on_select,
      .title = "Items",
  });

  const auto position_offset = m_window_frame->get_position_offset();
  m_items->position.x = position_offset.x;
  m_items->position.y = position_offset.y;
}

void SocietyInventory::process_input()
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

void SocietyInventory::set_items(const ItemList<entt::entity>& items)
{
  m_items->list->items = items;
  m_items->list->create_buttons();
}

void SocietyInventory::show()
{
  using namespace entt::literals;

  m_input_manager.push_context("inventory"_hs);
  m_context.ui_manager->bring_to_front(this);
  m_context.focused_stack->push_back(this);
  animate<AnimationFadeIn>(0.3, Easing::OutQuart);
}

void SocietyInventory::hide()
{
  animate<AnimationFadeOut>(0.3, Easing::OutQuart);
  m_context.focused_stack->pop_back();
  m_input_manager.pop_context();
}

}  // namespace dl::ui
