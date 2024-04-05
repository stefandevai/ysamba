#include "./action.hpp"

#include <spdlog/spdlog.h>

#include <entt/core/hashed_string.hpp>

#include "ai/action_manager.hpp"
#include "core/events/action.hpp"
#include "core/events/emitter.hpp"
#include "core/events/game.hpp"
#include "core/random.hpp"
#include "ecs/components/item.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/selectable.hpp"
#include "ecs/components/society_agent.hpp"
#include "ecs/systems/pickup.hpp"
#include "graphics/camera.hpp"
#include "graphics/quad.hpp"
#include "ui/compositions/action_menu.hpp"
#include "ui/compositions/notification.hpp"
#include "ui/ui_manager.hpp"
#include "world/world.hpp"

namespace dl
{
const ui::ItemList<uint32_t> ActionSystem::m_menu_items = {
    {0, "Harvest"},
    {1, "Break"},
    {2, "Dig"},
    {3, "Build hut"},
    {4, "Select storage area"},
};

ActionSystem::ActionSystem(World& world,
                           ui::UIManager& ui_manager,
                           EventEmitter& event_emitter,
                           ai::ActionManager& action_manager)
    : m_world(world), m_ui_manager(ui_manager), m_event_emitter(event_emitter), m_action_manager(action_manager)
{
  m_action_menu = m_ui_manager.emplace<ui::ActionMenu>(m_menu_items, m_on_select_generic_action);
}

void ActionSystem::update(entt::registry& registry, const Camera& camera)
{
  if (m_state == ActionMenuState::Open)
  {
    m_update_action_menu();
  }
  else if (m_state == ActionMenuState::Closed)
  {
    m_update_closed_menu(registry, camera);
  }
  else
  {
    m_update_selecting_target(registry, camera);
  }
}

void ActionSystem::m_update_action_menu()
{
  using namespace entt::literals;

  m_open_action_menu();

  if (m_action_menu->state != ui::UIComponent::State::Visible)
  {
    return;
  }

  if (m_input_manager.poll_action("close_menu"_hs))
  {
    m_dispose();
  }
  else if (m_input_manager.poll_action("harvest"_hs))
  {
    m_state = ActionMenuState::SelectHarvestTarget;
  }
  else if (m_input_manager.poll_action("break"_hs))
  {
    m_state = ActionMenuState::SelectBreakTarget;
  }
  else if (m_input_manager.poll_action("dig"_hs))
  {
    m_state = ActionMenuState::SelectDigTarget;
  }
}

void ActionSystem::m_update_closed_menu(entt::registry& registry, const Camera& camera)
{
  using namespace entt::literals;

  if (!m_input_manager.is_context("gameplay"_hs))
  {
    return;
  }

  if (m_input_manager.poll_action("open_action_menu"_hs))
  {
    m_action_menu->set_actions(m_menu_items);
    m_action_menu->set_on_select(m_on_select_generic_action);
    m_input_manager.push_context("action_menu"_hs);
    m_state = ActionMenuState::Open;
  }
  else if (m_input_manager.has_clicked(InputManager::MouseButton::Left))
  {
    const auto mouse_tile = m_world.mouse_to_world(camera);

    const auto selected_entity
        = m_world.spatial_hash.get_by_component<Selectable>(mouse_tile.x, mouse_tile.y, mouse_tile.z, registry);

    // If a selectable entity was clicked, toggle its selected state
    if (registry.valid(selected_entity))
    {
      auto& selectable = registry.get<Selectable>(selected_entity);
      selectable.selected = !selectable.selected;

      if (selectable.selected)
      {
        const auto& tile_size = m_world.get_tile_size();
        auto& quad = registry.emplace<Quad>(selected_entity, tile_size.x, tile_size.y, 0x5588cc88);
        quad.z_index = 4;
        m_selected_entities.push_back(selected_entity);
      }
      else
      {
        if (registry.all_of<Quad>(selected_entity))
        {
          registry.remove<Quad>(selected_entity);
        }
        m_selected_entities.erase(std::find(m_selected_entities.begin(), m_selected_entities.end(), selected_entity));
      }

      // Enter turn based mode if only one entity is selected
      if (m_selected_entities.size() == 1)
      {
        m_event_emitter.publish(EnterTurnBasedEvent{m_selected_entities[0]});
      }
      else
      {
        m_event_emitter.publish(LeaveTurnBasedEvent{});
      }
    }
    // If we are not selecting an entity, walk to the target
    else if (!m_selected_entities.empty())
    {
      m_action_manager.create_tile_job_bulk(JobType::Walk, m_selected_entities, mouse_tile);
    }
  }
  else if (m_input_manager.has_clicked(InputManager::MouseButton::Right))
  {
    assert(m_action_menu != nullptr);

    if (m_selected_entities.empty())
    {
      return;
    }

    const auto mouse_tile = m_world.mouse_to_world(camera);
    const auto selected_entity
        = m_world.spatial_hash.get_by_component<Item>(mouse_tile.x, mouse_tile.y, mouse_tile.z, registry);

    if (registry.valid(selected_entity))
    {
      // Get item actions
      const auto& item = registry.get<Item>(selected_entity);
      const auto& item_data = m_world.get_item_data(item.id);
      m_actions.clear();

      if (item_data.flags.contains("PICKABLE") && m_selected_entities.size() == 1
          && PickupSystem::can_pickup(registry, m_selected_entities[0], item_data))
      {
        m_actions.push_back({static_cast<uint32_t>(JobType::Pickup), "pickup"});
      }
      if (item_data.flags.contains("WEARABLE") && m_selected_entities.size() == 1)
      {
        m_actions.push_back({static_cast<uint32_t>(JobType::Wear), "wear"});
      }
      if (item_data.flags.contains("WIELDABLE") && m_selected_entities.size() == 1)
      {
        m_actions.push_back({static_cast<uint32_t>(JobType::Wield), "wield"});
      }

      m_action_menu->set_actions(m_actions);
      m_action_menu->set_on_select(
          [this, mouse_tile, selected_entity](const uint32_t i)
          {
            m_action_manager.create_item_job_bulk(
                static_cast<JobType>(i), m_selected_entities, selected_entity, mouse_tile);
            m_dispose();
          });
      m_input_manager.push_context("action_menu"_hs);
      m_state = ActionMenuState::Open;
    }
    else
    {
      // Get tile actions
      const auto& tile_data = m_world.get(mouse_tile.x, mouse_tile.y, mouse_tile.z);
      m_actions.clear();

      for (const auto& action : tile_data.actions)
      {
        m_actions.push_back({static_cast<uint32_t>(action.first), action.second.label});
      }

      m_action_menu->set_actions(m_actions);
      m_action_menu->set_on_select(
          [this, mouse_tile](const uint32_t i)
          {
            m_action_manager.create_tile_job_bulk(static_cast<JobType>(i), m_selected_entities, mouse_tile);
            m_dispose();
          });
      m_input_manager.push_context("action_menu"_hs);
      m_state = ActionMenuState::Open;
    }
  }
}

void ActionSystem::m_update_selecting_target(entt::registry& registry, const Camera& camera)
{
  using namespace entt::literals;

  m_close_action_menu();

  if (m_input_manager.poll_action("close_menu"_hs))
  {
    m_dispose();
    return;
  }

  switch (m_state)
  {
  case ActionMenuState::SelectHarvestTarget:
  {
    m_select_harvest_target(camera, registry);
    break;
  }
  case ActionMenuState::SelectBreakTarget:
  {
    m_select_break_target(camera, registry);
    break;
  }
  case ActionMenuState::SelectDigTarget:
  {
    m_select_dig_target(camera, registry);
    break;
  }
  case ActionMenuState::SelectHutTarget:
  {
    m_event_emitter.publish(SelectHutTargetEvent{});
    m_dispose();
    break;
  }
  case ActionMenuState::SelectStorageTarget:
  {
    m_event_emitter.publish(SelectStorageEvent{});
    m_dispose();
    break;
  }
  default:
  {
    m_dispose();
    break;
  }
  }
}

void ActionSystem::m_open_action_menu()
{
  assert(m_action_menu != nullptr);

  if (m_action_menu->state == ui::UIComponent::State::Hidden)
  {
    m_action_menu->show();
  }
}

void ActionSystem::m_close_action_menu()
{
  if (m_action_menu->state == ui::UIComponent::State::Visible)
  {
    m_action_menu->hide();
  }
}

void ActionSystem::m_dispose()
{
  if (m_notification != nullptr)
  {
    m_notification->hide();
    m_notification = nullptr;
  }

  m_close_action_menu();

  m_state = ActionMenuState::Closed;
  m_input_manager.pop_context();
}

void ActionSystem::m_select_harvest_target(const Camera& camera, entt::registry& registry)
{
  if (m_notification == nullptr)
  {
    m_notification = m_ui_manager.notify("Harvest where?");
  }

  if (m_input_manager.has_clicked(InputManager::MouseButton::Left))
  {
    const auto mouse_tile = m_world.mouse_to_world(camera);
    const bool created = m_action_manager.create_tile_job_bulk(JobType::Harvest, m_selected_entities, mouse_tile);

    if (created)
    {
      m_dispose();
    }
  }

  m_select_area(registry,
                camera,
                [this](entt::registry& registry, const Vector3i& begin, const Vector3i& end)
                {
                  std::vector<Vector3i> harvest_targets{};

                  for (int j = begin.y; j <= end.y; ++j)
                  {
                    for (int i = begin.x; i <= end.x; ++i)
                    {
                      const auto& tile = m_world.get(i, j, begin.z);

                      if (tile.actions.contains(JobType::Harvest))
                      {
                        harvest_targets.push_back({i, j, begin.z});
                      }
                    }
                  }

                  if (harvest_targets.empty())
                  {
                    return;
                  }

                  const auto entities = m_select_available_entities(registry);

                  for (const auto& target : harvest_targets)
                  {
                    const auto entity = random::select(entities);
                    m_action_manager.create_tile_job(JobType::Harvest, entity, target);
                  }

                  m_dispose();
                });
}

void ActionSystem::m_select_break_target(const Camera& camera, entt::registry& registry)
{
  if (m_notification == nullptr)
  {
    m_notification = m_ui_manager.notify("Break where?");
  }

  if (!m_input_manager.has_clicked(InputManager::MouseButton::Left))
  {
    return;
  }

  const auto mouse_tile = m_world.mouse_to_world(camera);
  const bool created = m_action_manager.create_tile_job_bulk(JobType::Break, m_selected_entities, mouse_tile);

  if (created)
  {
    m_dispose();
  }
}

void ActionSystem::m_select_dig_target(const Camera& camera, entt::registry& registry)
{
  if (m_notification == nullptr)
  {
    m_notification = m_ui_manager.notify("Dig where?");
  }

  if (!m_input_manager.has_clicked(InputManager::MouseButton::Left))
  {
    return;
  }

  const auto mouse_tile = m_world.mouse_to_world(camera);
  const bool created = m_action_manager.create_tile_job_bulk(JobType::Dig, m_selected_entities, mouse_tile);

  if (created)
  {
    m_dispose();
  }
}

void ActionSystem::m_select_area(entt::registry& registry,
                                 const Camera& camera,
                                 std::function<void(entt::registry&, const Vector3i&, const Vector3i&)> on_select)
{
  using namespace entt::literals;

  const auto& drag_bounds = m_input_manager.get_drag_bounds();

  Vector3i begin = m_world.screen_to_world(Vector2i{drag_bounds.x, drag_bounds.y}, camera);
  Vector3i end = m_world.screen_to_world(Vector2i{drag_bounds.z, drag_bounds.w}, camera);

  const Vector3i iteration_begin{std::min(begin.x, end.x), std::min(begin.y, end.y), begin.z};
  const Vector3i iteration_end{std::max(begin.x, end.x), std::max(begin.y, end.y), begin.z};

  if (m_input_manager.is_dragging())
  {
    if (iteration_begin == m_last_begin && iteration_end == m_last_end)
    {
      return;
    }

    m_last_begin = iteration_begin;
    m_last_end = iteration_end;

    m_preview_area(registry, iteration_begin, iteration_end);
  }
  else if (m_input_manager.has_dragged())
  {
    m_last_begin = Vector3i{};
    m_last_end = Vector3i{};

    for (const auto entity : registry.view<entt::tag<"area_preview"_hs>>())
    {
      registry.destroy(entity);
    }

    on_select(registry, iteration_begin, iteration_end);
  }
}

void ActionSystem::m_preview_area(entt::registry& registry, const Vector3i& begin, const Vector3i& end)
{
  using namespace entt::literals;

  assert(begin.x <= end.x && begin.y <= end.y);

  for (const auto entity : registry.view<entt::tag<"area_preview"_hs>>())
  {
    registry.destroy(entity);
  }

  const auto texture_id = m_world.get_spritesheet_id();

  Color preview_tile_color{0x66EEAA77};

  for (int j = begin.y; j <= end.y; ++j)
  {
    for (int i = begin.x; i <= end.x; ++i)
    {
      const auto entity = registry.create();
      const auto sprite = Sprite{
          .id = 2,
          .resource_id = texture_id,
          .layer_z = 4,
          .category = "tile",
          .color = preview_tile_color,
      };

      registry.emplace<Sprite>(entity, sprite);
      registry.emplace<Position>(entity, i, j, begin.z);
      registry.emplace<entt::tag<"area_preview"_hs>>(entity);
    }
  }
}

std::vector<entt::entity> ActionSystem::m_select_available_entities(entt::registry& registry)
{
  std::vector<entt::entity> free_entities;
  std::vector<entt::entity> selected_entities;

  for (const auto entity : registry.view<SocietyAgent, Selectable>())
  {
    const auto& agent = registry.get<SocietyAgent>(entity);
    const auto& selectable = registry.get<Selectable>(entity);

    if (selectable.selected)
    {
      selected_entities.push_back(entity);
    }

    if (!agent.jobs.empty())
    {
      continue;
    }

    free_entities.push_back(entity);
  }

  if (!selected_entities.empty())
  {
    return selected_entities;
  }

  return free_entities;
}

}  // namespace dl
