#include "./action.hpp"

#include <spdlog/spdlog.h>

#include <entt/core/hashed_string.hpp>

#include "ai/actions/generic_item.hpp"
#include "ai/actions/generic_tile.hpp"
#include "ai/actions/utils.hpp"
#include "ai/actions/walk.hpp"
#include "core/events/action.hpp"
#include "core/events/emitter.hpp"
#include "core/events/game.hpp"
#include "core/maths/random.hpp"
#include "ecs/components/item.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/selectable.hpp"
#include "ecs/components/society_agent.hpp"
#include "ecs/systems/pickup.hpp"
#include "graphics/camera.hpp"
#include "graphics/quad.hpp"
#include "ui/compositions/action_menu.hpp"
#include "ui/compositions/notification.hpp"
#include "ui/gameplay_modals.hpp"
#include "ui/ui_manager.hpp"
#include "world/world.hpp"

namespace dl
{
const ui::ItemList<JobType> ActionSystem::m_menu_items = {
    {JobType::Harvest, "Harvest"},
    {JobType::Break, "Break"},
    {JobType::Dig, "Dig"},
    {JobType::BuildHut, "Build hut"},
    {JobType::SelectStorageArea, "Select storage area"},
};

ActionSystem::ActionSystem(World& world,
                           ui::UIManager& ui_manager,
                           EventEmitter& event_emitter,
                           ui::GameplayModals& gameplay_modals)
    : m_world(world), m_gameplay_modals(gameplay_modals), m_ui_manager(ui_manager), m_event_emitter(event_emitter)
{
  m_gameplay_modals.action_menu->set_on_select([this](JobType job_type) { m_on_select_generic_action(job_type); });
  m_gameplay_modals.action_menu->set_actions(m_menu_items);
}

void ActionSystem::update(entt::registry& registry, const Camera& camera)
{
  switch (m_ui_state)
  {
  case UIState::None:
  {
    m_process_input(registry, camera);
    break;
  }
  case UIState::SelectingTarget:
  {
    m_update_selecting_target(registry, camera);
    break;
  }
  default:
    break;
  }
}

void ActionSystem::m_process_input(entt::registry& registry, const Camera& camera)
{
  using namespace entt::literals;

  if (!m_input_manager.is_context("gameplay"_hs))
  {
    return;
  }

  if (m_input_manager.poll_action("open_action_menu"_hs))
  {
    m_gameplay_modals.action_menu->set_actions(m_menu_items);
    m_gameplay_modals.action_menu->set_on_select([this](JobType job_type) { m_on_select_generic_action(job_type); });
    m_gameplay_modals.action_menu->show();
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
      for (const auto entity : m_selected_entities)
      {
        action::walk::job({
            .registry = registry,
            .agent_entity = entity,
            .position = mouse_tile,
        });
      }
    }
  }
  else if (m_input_manager.has_clicked(InputManager::MouseButton::Right))
  {
    assert(m_gameplay_modals.action_menu != nullptr);

    if (m_selected_entities.empty())
    {
      return;
    }

    const auto mouse_tile = m_world.mouse_to_world(camera);
    const auto& tile_data = m_world.get(mouse_tile.x, mouse_tile.y, mouse_tile.z);
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
        m_actions.push_back({JobType::Pickup, "pickup"});
      }
      if (item_data.flags.contains("WEARABLE") && m_selected_entities.size() == 1)
      {
        m_actions.push_back({JobType::Wear, "wear"});
      }
      if (item_data.flags.contains("WIELDABLE") && m_selected_entities.size() == 1)
      {
        m_actions.push_back({JobType::Wield, "wield"});
      }
      if (tile_data.flags.contains("WALKABLE"))
      {
        m_actions.push_back({JobType::Walk, "walk to location"});
      }

      m_gameplay_modals.action_menu->set_actions(m_actions);
      m_gameplay_modals.action_menu->set_on_select(
          [this, mouse_tile, selected_entity, &registry](const JobType job_type)
          {
            m_dispatch_action(registry, job_type, mouse_tile, selected_entity);
            m_dispose();
          });
      m_gameplay_modals.action_menu->show();
    }
    else
    {
      // Get tile actions
      m_actions.clear();

      for (const auto& action : tile_data.actions)
      {
        m_actions.push_back({action.first, action.second.label});
      }

      if (tile_data.flags.contains("WALKABLE"))
      {
        m_actions.push_back({JobType::Walk, "walk to location"});
      }

      m_gameplay_modals.action_menu->set_actions(m_actions);
      m_gameplay_modals.action_menu->set_on_select(
          [this, mouse_tile, &registry](const JobType job_type)
          {
            m_dispatch_action(registry, job_type, mouse_tile);
            m_dispose();
          });
      m_gameplay_modals.action_menu->show();
    }
  }
}

void ActionSystem::m_on_select_generic_action(JobType job_type)
{
  m_selected_job_type = job_type;
  m_ui_state = UIState::SelectingTarget;
  m_gameplay_modals.action_menu->hide();
}

void ActionSystem::m_update_selecting_target(entt::registry& registry, const Camera& camera)
{
  using namespace entt::literals;

  switch (m_selected_job_type)
  {
  case JobType::Harvest:
  {
    m_select_harvest_target(camera, registry);
    break;
  }
  case JobType::Break:
  {
    m_select_break_target(camera, registry);
    break;
  }
  case JobType::Dig:
  {
    m_select_dig_target(camera, registry);
    break;
  }
  case JobType::BuildHut:
  {
    m_event_emitter.publish(SelectHutTargetEvent{});
    m_dispose();
    break;
  }
  case JobType::SelectStorageArea:
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

void ActionSystem::m_dispose()
{
  if (m_notification != nullptr)
  {
    m_notification->hide();
    m_notification = nullptr;
    m_input_manager.pop_context();
  }

  if (m_gameplay_modals.action_menu->is_visible())
  {
    m_gameplay_modals.action_menu->hide();
  }

  m_ui_state = UIState::None;
  m_selected_job_type = JobType::None;
}

void ActionSystem::m_select_harvest_target(const Camera& camera, entt::registry& registry)
{
  using namespace entt::literals;

  if (m_notification == nullptr)
  {
    m_notification = m_ui_manager.notify("Harvest where?");
    m_input_manager.push_context("notification"_hs);
  }

  // Process input for selecting target notification
  if (m_input_manager.poll_action("close"_hs))
  {
    m_dispose();
    return;
  }

  if (m_input_manager.has_clicked(InputManager::MouseButton::Left))
  {
    const auto mouse_tile = m_world.mouse_to_world(camera);
    const bool created = action::generic_tile::job({
        .world = m_world,
        .registry = registry,
        .position = mouse_tile,
        .job_type = JobType::Harvest,
        .entities = m_selected_entities,
    });

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
                    action::generic_tile::job({
                        .world = m_world,
                        .registry = registry,
                        .position = target,
                        .job_type = JobType::Harvest,
                        .entities = {entity},
                    });
                  }

                  m_dispose();
                });
}

void ActionSystem::m_select_break_target(const Camera& camera, entt::registry& registry)
{
  using namespace entt::literals;

  if (m_notification == nullptr)
  {
    m_notification = m_ui_manager.notify("Break where?");
    m_input_manager.push_context("notification"_hs);
  }

  // Process input for selecting target notification
  if (m_input_manager.poll_action("close"_hs))
  {
    m_dispose();
    return;
  }

  if (!m_input_manager.has_clicked(InputManager::MouseButton::Left))
  {
    return;
  }

  const auto mouse_tile = m_world.mouse_to_world(camera);
  const bool created = action::generic_tile::job({
      .world = m_world,
      .registry = registry,
      .position = mouse_tile,
      .job_type = JobType::Break,
      .entities = m_selected_entities,
  });

  if (created)
  {
    m_dispose();
    return;
  }
}

void ActionSystem::m_select_dig_target(const Camera& camera, entt::registry& registry)
{
  using namespace entt::literals;

  if (m_notification == nullptr)
  {
    m_notification = m_ui_manager.notify("Dig where?");
    m_input_manager.push_context("notification"_hs);
  }

  // Process input for selecting target notification
  if (m_input_manager.poll_action("close"_hs))
  {
    m_dispose();
    return;
  }

  if (!m_input_manager.has_clicked(InputManager::MouseButton::Left))
  {
    return;
  }

  const auto mouse_tile = m_world.mouse_to_world(camera);
  const bool created = action::generic_tile::job({
      .world = m_world,
      .registry = registry,
      .position = mouse_tile,
      .job_type = JobType::Dig,
      .entities = m_selected_entities,
  });

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

void ActionSystem::m_dispatch_action(entt::registry& registry,
                                     JobType job_type,
                                     const Vector3i& position,
                                     entt::entity item)
{
  switch (job_type)
  {
  // Generic tile actions
  case JobType::Harvest:
  case JobType::Break:
  case JobType::Dig:
  case JobType::PrepareFirecamp:
  case JobType::StartFire:
  case JobType::PlaceHutExterior:
  case JobType::ConstructEntrance:
  case JobType::PickupLiquid:
  {
    action::generic_tile::job({
        .world = m_world,
        .registry = registry,
        .position = position,
        .job_type = job_type,
        .entities = m_selected_entities,
    });
    break;
  }

  // Generic item actions
  case JobType::Pickup:
  case JobType::Wear:
  case JobType::Wield:
  case JobType::Drop:
  {
    if (item == entt::null)
    {
      spdlog::critical("No item selected for action");
      break;
    }

    action::generic_item::job({
        .registry = registry,
        .position = position,
        .job_type = job_type,
        .entities = m_selected_entities,
        .item = item,
    });
    break;
  }

  // Specific actions
  case JobType::Walk:
  {
    for (const auto entity : m_selected_entities)
    {
      action::walk::job({
          .registry = registry,
          .agent_entity = entity,
          .position = position,
      });
    }
    break;
  }

  default:
  {
    spdlog::critical("Unknown job: {}", static_cast<int>(job_type));
    break;
  }
  }
}

}  // namespace dl
