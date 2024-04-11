#include "./item_selection.hpp"

#include <spdlog/spdlog.h>

#include "core/maths/vector.hpp"
#include "ui/animation.hpp"
#include "ui/components/label.hpp"
#include "ui/components/window_frame.hpp"

namespace dl::ui
{
ItemSelection::ItemSelection(UIContext& context, Params params)
    : UIComponent(context, "ItemSelection"), m_world(params.world)
{
  state = UIComponent::State::Hidden;
  size = Vector2i{300, 400};
  x_alignment = XAlignement::Center;
  y_alignment = YAlignement::Center;

  m_window_frame = emplace<WindowFrame>(WindowFrame::Params{
      .size = size,
  });

  m_items = m_window_frame->emplace<ScrollableTextButtonList<EntityPair>>(ScrollableTextButtonList<EntityPair>::Params{
      .items = std::move(params.items),
      .on_left_click = std::move(params.on_select),
      .size = m_window_frame->get_safe_area_size(),
      .title = std::move(params.title),
      .enumerate = true,
      .enumeration_type = EnumerationType::Alphabetical,
  });

  const auto position_offset = m_window_frame->get_position_offset();
  m_items->position.x = position_offset.x;
  m_items->position.y = position_offset.y;
}

void ItemSelection::process_input()
{
  using namespace entt::literals;

  if (!m_input_manager.is_context("list_selection"_hs))
  {
    return;
  }

  if (m_input_manager.poll_action("close"_hs))
  {
    hide();
  }
}

void ItemSelection::show()
{
  using namespace entt::literals;

  m_input_manager.push_context("list_selection"_hs);
  m_items->scrollable->reset_scroll();
  animate<AnimationFadeIn>(0.3, Easing::OutQuart);
}

void ItemSelection::hide()
{
  m_input_manager.pop_context();
  animate<AnimationFadeOut>(0.3, Easing::OutQuart);
}

void ItemSelection::set_items(const ItemList<EntityPair> items)
{
  m_items->list->items = std::move(items);
  m_items->list->create_buttons();
}

void ItemSelection::set_on_select(const std::function<void(const EntityPair&)> on_select)
{
  m_items->list->on_left_click = std::move(on_select);
}

void ItemSelection::set_items_from_entity(entt::registry& registry, entt::entity entity)
{
  auto& all_items = m_items->list->items;

  all_items.clear();

  const auto entity_items = utils::get_entity_items(registry, entity);

  for (const auto item_entity : entity_items)
  {
    all_items.push_back({{entity, item_entity}, item_factory::get_item_label(m_world, registry, item_entity)});
  }

  m_items->list->create_buttons();
}

void ItemSelection::set_title(const std::string title)
{
  m_items->title_label->set_text(std::move(title));
}


}  // namespace dl::ui
