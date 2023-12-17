#include "./drop.hpp"

#include <spdlog/spdlog.h>

#include <entt/core/hashed_string.hpp>

#include "ecs/components/carried_items.hpp"
#include "ecs/components/item.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/rectangle.hpp"
#include "ecs/components/selectable.hpp"
#include "ecs/components/society_agent.hpp"
#include "ecs/components/weared_items.hpp"
#include "ecs/components/wielded_items.hpp"
#include "graphics/camera.hpp"
#include "graphics/text.hpp"
#include "ui/components/label.hpp"
#include "ui/compositions/item_selection.hpp"
#include "ui/ui_manager.hpp"
#include "world/world.hpp"

namespace dl
{
DropSystem::DropSystem(World& world, ui::UIManager& ui_manager) : m_world(world), m_ui_manager(ui_manager)
{
  const auto on_select = [this](const uint32_t entity) { m_state = DropMenuState::SelectingTarget; };

  m_drop_menu = m_ui_manager.emplace<ui::ItemSelection>(std::move(on_select));

  m_select_target_label = m_ui_manager.emplace<ui::Label>("Select target");
  m_select_target_label->state = ui::UIComponent::State::Hidden;
  m_select_target_label->x_alignment = ui::XAlignement::Center;
  m_select_target_label->position.y = 30;
}

void DropSystem::update(entt::registry& registry, const Camera& camera)
{
  if (m_state == DropMenuState::Open)
  {
    m_update_drop_menu();
  }
  else if (m_state == DropMenuState::Closed)
  {
    m_update_closed_menu(registry, camera);
  }
  else
  {
    m_update_selecting_target(registry, camera);
  }
}

void DropSystem::m_update_drop_menu()
{
  using namespace entt::literals;

  m_open_drop_menu();

  if (m_input_manager.poll_action("close_menu"_hs))
  {
    m_dispose();
  }
}

void DropSystem::m_update_closed_menu(entt::registry& registry, const Camera& camera)
{
  using namespace entt::literals;

  if (!m_input_manager.is_context("gameplay"_hs))
  {
    return;
  }

  if (m_input_manager.poll_action("drop_item"_hs))
  {
    m_items.clear();
    std::vector<entt::entity> selected_entities{};
    auto view = registry.view<Selectable>();

    for (const auto entity : view)
    {
      const auto& selectable = registry.get<Selectable>(entity);

      if (!selectable.selected)
      {
        continue;
      }

      selected_entities.push_back(entity);
    }

    if (selected_entities.empty())
    {
      return;
    }

    for (const auto entity : selected_entities)
    {
      if (registry.all_of<WieldedItems>(entity))
      {
        const auto& items = registry.get<WieldedItems>(entity);
        const auto left_hand = items.left_hand;
        const auto right_hand = items.right_hand;

        if (registry.valid(left_hand))
        {
          const auto& item_component = registry.get<Item>(left_hand);
          const auto& item = m_world.get_item_data(item_component.id);
          m_items.push_back({static_cast<uint32_t>(left_hand), item.name});
        }
        if (registry.valid(right_hand) && right_hand != left_hand)
        {
          const auto& item_component = registry.get<Item>(right_hand);
          const auto& item = m_world.get_item_data(item_component.id);
          m_items.push_back({static_cast<uint32_t>(right_hand), item.name});
        }
      }
      if (registry.all_of<WearedItems>(entity))
      {
        const auto& items = registry.get<WearedItems>(entity);

        for (auto item_entity : items.items)
        {
          const auto& item_component = registry.get<Item>(item_entity);
          const auto& item = m_world.get_item_data(item_component.id);
          m_items.push_back({static_cast<uint32_t>(item_entity), item.name});
        }
      }
      if (registry.all_of<CarriedItems>(entity))
      {
        const auto& items = registry.get<CarriedItems>(entity);

        for (auto item_entity : items.items)
        {
          const auto& item_component = registry.get<Item>(item_entity);
          const auto& item = m_world.get_item_data(item_component.id);
          m_items.push_back({static_cast<uint32_t>(item_entity), item.name});
        }
      }
    }

    m_drop_menu->set_items(m_items);
    m_state = DropMenuState::Open;
    m_input_manager.push_context("item_selection_menu"_hs);
  }
}

void DropSystem::m_update_selecting_target(entt::registry& registry, const Camera& camera)
{
  using namespace entt::literals;

  m_close_drop_menu();
  m_show_select_target_text();

  if (m_input_manager.poll_action("close_menu"_hs))
  {
    m_dispose();
  }
  else if (m_input_manager.has_clicked(InputManager::MouseButton::Left))
  {
    /* if (!registry.valid(agent_entity) || !registry.valid(item_entity)) */
    /* { */
    /*   return; */
    /* } */

    const auto& mouse_position = m_input_manager.get_mouse_position();
    const auto& camera_position = camera.get_position();
    const auto& tile_size = camera.get_tile_size();

    Vector2i tile_position{};
    tile_position.x = (mouse_position.x + camera_position.x) / tile_size.x;
    tile_position.y = (mouse_position.y + camera_position.y) / tile_size.y;

    /* auto& agent = registry.get<SocietyAgent>(entity); */
    /* agent.jobs.push(Job{JobType::Walk, 2, Target{Vector3i{position.x, position.y, 0}}}); */
    /* agent.jobs.push(Job{job_type, 2, Target{Vector3i{position.x, position.y, 0}, id}}); */

    /* switch (m_state) */
    /* { */
    /* default: */
    /* { */
    /*   m_dispose(); */
    /*   break; */
    /* } */
    /* } */
    m_dispose();
  }
}

void DropSystem::m_open_drop_menu()
{
  assert(m_drop_menu != nullptr);

  if (m_drop_menu->state == ui::UIComponent::State::Hidden)
  {
    m_drop_menu->show();
  }
}

void DropSystem::m_show_select_target_text()
{
  assert(m_select_target_label != nullptr);

  if (m_select_target_label->state == ui::UIComponent::State::Hidden)
  {
    m_select_target_label->show();
  }
}

void DropSystem::m_close_drop_menu()
{
  if (m_drop_menu->state == ui::UIComponent::State::Visible)
  {
    m_drop_menu->hide();
  }
}

void DropSystem::m_close_select_target()
{
  if (m_select_target_label->state == ui::UIComponent::State::Visible)
  {
    m_select_target_label->hide();
  }
}

void DropSystem::m_dispose()
{
  m_close_drop_menu();
  m_close_select_target();

  m_state = DropMenuState::Closed;
  m_input_manager.pop_context();
}

void DropSystem::m_create_job(
    const JobType job_type, const uint32_t id, const Vector2i& position, entt::registry& registry, entt::entity entity)
{
  auto& agent = registry.get<SocietyAgent>(entity);
  agent.jobs.push(Job{JobType::Walk, 2, Target{Vector3i{position.x, position.y, 0}}});
  agent.jobs.push(Job{job_type, 2, Target{Vector3i{position.x, position.y, 0}, id}});
}

}  // namespace dl
