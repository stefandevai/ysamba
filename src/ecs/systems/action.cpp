#include "./action.hpp"

#include <spdlog/spdlog.h>

#include <entt/core/hashed_string.hpp>

#include "core/events/emitter.hpp"
#include "core/events/game.hpp"
#include "core/random.hpp"
#include "ecs/components/action_pickup.hpp"
#include "ecs/components/carried_items.hpp"
#include "ecs/components/item.hpp"
#include "ecs/components/job_data.hpp"
#include "ecs/components/job_data_build_hut.hpp"
#include "ecs/components/job_progress.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/selectable.hpp"
#include "ecs/components/society_agent.hpp"
#include "ecs/components/weared_items.hpp"
#include "ecs/components/wielded_items.hpp"
#include "ecs/systems/pickup.hpp"
#include "graphics/camera.hpp"
#include "graphics/quad.hpp"
#include "ui/components/label.hpp"
#include "ui/compositions/action_menu.hpp"
#include "ui/ui_manager.hpp"
#include "world/society/job.hpp"
#include "world/world.hpp"

namespace dl
{
const ui::ItemList<uint32_t> ActionSystem::m_menu_items = {
    {0, "Harvest"},
    {1, "Break"},
    {2, "Dig"},
    {3, "Build hut"},
};

ActionSystem::ActionSystem(World& world, ui::UIManager& ui_manager, EventEmitter& event_emitter)
    : m_world(world), m_ui_manager(ui_manager), m_event_emitter(event_emitter)
{
  m_action_menu = m_ui_manager.emplace<ui::ActionMenu>(m_menu_items, m_on_select_generic_action);

  m_select_target_label = m_ui_manager.emplace<ui::Label>("Select target");
  m_select_target_label->state = ui::UIComponent::State::Hidden;
  m_select_target_label->x_alignment = ui::XAlignement::Center;
  m_select_target_label->position.y = 30;
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
      for (const auto entity : m_selected_entities)
      {
        const auto job_entity = registry.create();
        registry.emplace<Target>(job_entity, mouse_tile, 0, 0);
        registry.emplace<JobData>(job_entity, JobType::Walk);

        auto& agent = registry.get<SocietyAgent>(entity);
        agent.jobs.push(Job{0, job_entity});
      }
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
      m_action_menu->set_on_select([this, &registry, mouse_tile, selected_entity](const uint32_t i) {
        const auto job = registry.create();
        registry.emplace<Target>(job, mouse_tile, static_cast<uint32_t>(selected_entity));
        registry.emplace<JobData>(job, static_cast<JobType>(i));

        for (const auto entity : m_selected_entities)
        {
          m_assign_job(job, mouse_tile, registry, entity);
        }
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
      m_action_menu->set_on_select([this, &registry, mouse_tile, &tile_data](const uint32_t i) {
        const auto job = registry.create();
        registry.emplace<Target>(job, mouse_tile, tile_data.id);
        registry.emplace<JobData>(job, static_cast<JobType>(i));

        for (const auto entity : m_selected_entities)
        {
          m_assign_job(job, mouse_tile, registry, entity);
        }
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

  m_show_select_target_text();

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
    m_select_hut_target(camera, registry);
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

void ActionSystem::m_show_select_target_text()
{
  assert(m_select_target_label != nullptr);

  if (m_select_target_label->state == ui::UIComponent::State::Hidden)
  {
    m_select_target_label->show();
  }
}

void ActionSystem::m_close_action_menu()
{
  if (m_action_menu->state == ui::UIComponent::State::Visible)
  {
    m_action_menu->hide();
  }
}

void ActionSystem::m_close_select_target()
{
  if (m_select_target_label->state == ui::UIComponent::State::Visible)
  {
    m_select_target_label->hide();
  }
}

void ActionSystem::m_dispose()
{
  m_close_action_menu();
  m_close_select_target();

  m_state = ActionMenuState::Closed;
  m_input_manager.pop_context();
}

void ActionSystem::m_select_tile_target(const Vector3i& tile_position, const JobType job_type, entt::registry& registry)
{
  const auto& tile = m_world.get(tile_position.x, tile_position.y, tile_position.z);

  if (tile.actions.contains(job_type))
  {
    const auto& qualities_required = tile.actions.at(job_type).qualities_required;
    const auto& consumed_items = tile.actions.at(job_type).consumes;

    if (!consumed_items.empty())
    {
      if (!m_has_consumables(consumed_items, registry))
      {
        // TODO: Notify player that items are needed
        return;
      }
    }

    const auto job = registry.create();
    registry.emplace<Target>(job, tile_position, tile.id);
    registry.emplace<JobData>(job, job_type);
    bool job_added = false;

    for (const auto entity : m_selected_entities)
    {
      // Check if the agent has the necessary qualities to perform the action
      if (!qualities_required.empty())
      {
        if (!m_has_qualities_required(qualities_required, entity, registry))
        {
          // TODO: Notify player that items with required qualities are needed
          continue;
        }
      }

      m_assign_job(job, tile_position, registry, entity);
      job_added = true;
    }

    if (!job_added)
    {
      registry.destroy(job);
    }
    m_dispose();
  }
}

void ActionSystem::m_select_harvest_target(const Camera& camera, entt::registry& registry)
{
  if (!m_input_manager.has_clicked(InputManager::MouseButton::Left))
  {
    return;
  }

  const auto mouse_tile = m_world.mouse_to_world(camera);
  m_select_tile_target(mouse_tile, JobType::Harvest, registry);
}

void ActionSystem::m_select_break_target(const Camera& camera, entt::registry& registry)
{
  if (!m_input_manager.has_clicked(InputManager::MouseButton::Left))
  {
    return;
  }

  const auto mouse_tile = m_world.mouse_to_world(camera);
  m_select_tile_target(mouse_tile, JobType::Break, registry);
}

void ActionSystem::m_select_dig_target(const Camera& camera, entt::registry& registry)
{
  if (!m_input_manager.has_clicked(InputManager::MouseButton::Left))
  {
    return;
  }

  const auto mouse_tile = m_world.mouse_to_world(camera);
  m_select_tile_target(mouse_tile, JobType::Dig, registry);
}

void ActionSystem::m_select_hut_target(const Camera& camera, entt::registry& registry)
{
  const auto& tile_size = m_world.get_tile_size();
  const auto& drag_bounds = m_input_manager.get_drag_bounds();

  Vector2i area{std::ceil(std::abs(drag_bounds.z - drag_bounds.x) / static_cast<float>(tile_size.x)),
                std::ceil(std::abs(drag_bounds.w - drag_bounds.y) / static_cast<float>(tile_size.y))};

  const uint32_t hut_size = std::max(std::max(area.x, area.y), 3);

  Vector2i direction;
  direction.x = drag_bounds.x <= drag_bounds.z ? 0 : 1;
  direction.y = drag_bounds.y <= drag_bounds.w ? 0 : 1;

  Vector3i begin = m_world.screen_to_world(Vector2i{drag_bounds.x, drag_bounds.y}, camera);

  begin.x -= hut_size * direction.x;
  begin.y -= hut_size * direction.y;

  if (m_input_manager.is_dragging())
  {
    m_preview_hut_target(begin, hut_size, registry);
  }
  else if (m_input_manager.has_dragged())
  {
    m_create_hut_job(begin, hut_size, registry);
  }
}

void ActionSystem::m_preview_hut_target(const Vector3i& tile_position,
                                        const uint32_t hut_size,
                                        entt::registry& registry)
{
  assert(hut_size >= 3);

  for (const auto entity : registry.view<entt::tag<"hut_preview"_hs>>())
  {
    registry.destroy(entity);
  }

  const auto texture_id = m_world.get_texture_id();

  Color sprite_color{0xFFFFFF99};
  if (!m_can_build_hut(hut_size, tile_position))
  {
    sprite_color = Color{0xFF0000FF};
  }

  auto add_hut_part
      = [&registry, &sprite_color, texture_id](const uint32_t id, const double x, const double y, const double z) {
          const auto entity = registry.create();
          registry.emplace<entt::tag<"hut_preview"_hs>>(entity);
          const auto sprite = Sprite{
              .id = id,
              .resource_id = texture_id,
              .layer_z = 4,
              .category = "tile",
              .color = sprite_color,
          };
          registry.emplace<Sprite>(entity, sprite);
          registry.emplace<Position>(entity, x, y, z);
        };

  if (hut_size == 3)
  {
    // Perimeter
    add_hut_part(139, tile_position.x, tile_position.y, tile_position.z);
    add_hut_part(140, tile_position.x + 1, tile_position.y, tile_position.z);
    add_hut_part(141, tile_position.x + 2, tile_position.y, tile_position.z);
    add_hut_part(142, tile_position.x, tile_position.y + 1, tile_position.z);
    add_hut_part(144, tile_position.x + 2, tile_position.y + 1, tile_position.z);
    add_hut_part(145, tile_position.x, tile_position.y + 2, tile_position.z);
    add_hut_part(146, tile_position.x + 1, tile_position.y + 2, tile_position.z);
    add_hut_part(147, tile_position.x + 2, tile_position.y + 2, tile_position.z);

    // Top
    add_hut_part(148, tile_position.x + 1, tile_position.y + 1, tile_position.z + 1);
  }
  else if (hut_size == 4)
  {
    // Perimeter
    add_hut_part(149, tile_position.x, tile_position.y, tile_position.z);
    add_hut_part(155, tile_position.x + 3, tile_position.y, tile_position.z);
    add_hut_part(152, tile_position.x, tile_position.y + 1, tile_position.z);
    add_hut_part(153, tile_position.x + 3, tile_position.y + 1, tile_position.z);
    add_hut_part(175, tile_position.x, tile_position.y + 2, tile_position.z);
    add_hut_part(181, tile_position.x + 3, tile_position.y + 2, tile_position.z);
    add_hut_part(182, tile_position.x, tile_position.y + 3, tile_position.z);
    add_hut_part(183, tile_position.x + 1, tile_position.y + 3, tile_position.z);
    add_hut_part(187, tile_position.x + 2, tile_position.y + 3, tile_position.z);
    add_hut_part(188, tile_position.x + 3, tile_position.y + 3, tile_position.z);

    // Top
    add_hut_part(150, tile_position.x + 1, tile_position.y + 1, tile_position.z + 1);
    add_hut_part(154, tile_position.x + 2, tile_position.y + 1, tile_position.z + 1);
    add_hut_part(169, tile_position.x + 1, tile_position.y + 2, tile_position.z + 1);
    add_hut_part(173, tile_position.x + 2, tile_position.y + 2, tile_position.z + 1);
  }
  else
  {
    // Perimeter structure parts
    add_hut_part(149, tile_position.x, tile_position.y, tile_position.z);
    add_hut_part(155, tile_position.x + hut_size - 1, tile_position.y, tile_position.z);
    add_hut_part(156, tile_position.x, tile_position.y + 1, tile_position.z);
    add_hut_part(159, tile_position.x + hut_size - 1, tile_position.y + 1, tile_position.z);

    add_hut_part(175, tile_position.x, tile_position.y + hut_size - 2, tile_position.z);
    add_hut_part(181, tile_position.x + hut_size - 1, tile_position.y + hut_size - 2, tile_position.z);
    add_hut_part(182, tile_position.x, tile_position.y + hut_size - 1, tile_position.z);
    add_hut_part(183, tile_position.x + 1, tile_position.y + hut_size - 1, tile_position.z);
    add_hut_part(187, tile_position.x + hut_size - 2, tile_position.y + hut_size - 1, tile_position.z);
    add_hut_part(188, tile_position.x + hut_size - 1, tile_position.y + hut_size - 1, tile_position.z);

    // Top structure parts
    add_hut_part(150, tile_position.x + 1, tile_position.y + 1, tile_position.z + 1);
    add_hut_part(154, tile_position.x + hut_size - 2, tile_position.y + 1, tile_position.z + 1);
    add_hut_part(169, tile_position.x + 1, tile_position.y + hut_size - 2, tile_position.z + 1);
    add_hut_part(173, tile_position.x + hut_size - 2, tile_position.y + hut_size - 2, tile_position.z + 1);

    // Tiled parts
    for (uint32_t i = 2; i < hut_size - 2; ++i)
    {
      // Horizontal structure parts
      add_hut_part(151, tile_position.x + i, tile_position.y + 1, tile_position.z + 1);
      add_hut_part(170, tile_position.x + i, tile_position.y + hut_size - 2, tile_position.z + 1);
      add_hut_part(184, tile_position.x + i, tile_position.y + hut_size - 1, tile_position.z);

      // Vertical structure parts
      add_hut_part(157, tile_position.x + 1, tile_position.y + i, tile_position.z + 1);
      add_hut_part(158, tile_position.x + hut_size - 2, tile_position.y + i, tile_position.z + 1);
      add_hut_part(160, tile_position.x, tile_position.y + i, tile_position.z);
      add_hut_part(163, tile_position.x + hut_size - 1, tile_position.y + i, tile_position.z);
    }
  }
}

void ActionSystem::m_create_hut_job(const Vector3i& tile_position, const uint32_t hut_size, entt::registry& registry)
{
  using namespace entt::literals;

  if (!m_can_build_hut(hut_size, tile_position))
  {
    for (const auto entity : registry.view<entt::tag<"hut_preview"_hs>>())
    {
      registry.destroy(entity);
    }
    return;
  }

  // Create progress entity
  const uint32_t cost_per_tile = 200;
  const uint32_t total_cost = cost_per_tile * hut_size * hut_size;
  const auto job_progress_entity = registry.create();
  auto& progress = registry.emplace<JobProgress>(job_progress_entity, JobType::BuildHut, total_cost);
  registry.emplace<Position>(job_progress_entity,
                             static_cast<double>(tile_position.x),
                             static_cast<double>(tile_position.y),
                             static_cast<double>(tile_position.z));

  // Assign a build hut job for each agent
  auto assign_build_hut_job
      = [&registry, &tile_position, &progress, hut_size, job_progress_entity](const entt::entity entity) {
          const auto offset_x = random::get_integer(0, hut_size);
          const auto offset_y = random::get_integer(0, hut_size);
          const auto job_target = Vector3i{tile_position.x + offset_x, tile_position.y + offset_y, tile_position.z};

          // Create a walk job to walk until the target
          const auto walk_job = registry.create();
          registry.emplace<Target>(walk_job, job_target);
          registry.emplace<JobData>(walk_job, JobType::Walk);

          // Create the main job
          const auto build_hut_job = registry.create();
          registry.emplace<JobDataBuildHut>(build_hut_job, hut_size);

          // Assign the progress entity to the job
          auto& job_data = registry.emplace<JobData>(build_hut_job, JobType::BuildHut);
          job_data.progress_entity = job_progress_entity;

          auto& agent = registry.get<SocietyAgent>(entity);
          agent.jobs.push(Job{2, walk_job});
          agent.jobs.push(Job{2, build_hut_job});

          progress.agents.push_back(entity);
        };

  if (m_selected_entities.empty())
  {
    auto entities = m_select_available_entities(registry);
    std::for_each(entities.begin(), entities.end(), assign_build_hut_job);
  }
  else
  {
    std::for_each(m_selected_entities.begin(), m_selected_entities.end(), assign_build_hut_job);
  }

  m_dispose();
}

bool ActionSystem::m_can_build_hut(const uint32_t hut_size, const Vector3i& position)
{
  if (hut_size < 3)
  {
    return false;
  }

  // Check if soil is buildable
  for (uint32_t j = 0; j < hut_size; ++j)
  {
    for (uint32_t i = 0; i < hut_size; ++i)
    {
      if (!m_world.is_walkable(position.x + i, position.y + j, position.z))
      {
        return false;
      }
    }
  }

  // Check if there's space over the ground to build the hut
  for (uint32_t j = 0; j < hut_size; ++j)
  {
    for (uint32_t i = 0; i < hut_size; ++i)
    {
      if (!m_world.is_empty(position.x + i, position.y + j, position.z + 1))
      {
        return false;
      }
    }
  }

  return true;
}

void ActionSystem::m_assign_job(const entt::entity job,
                                const Vector3i& position,
                                entt::registry& registry,
                                const entt::entity entity)
{
  const auto walk_job = registry.create();
  registry.emplace<Target>(walk_job, position);
  registry.emplace<JobData>(walk_job, JobType::Walk);

  auto& agent = registry.get<SocietyAgent>(entity);
  agent.jobs.push(Job{2, walk_job});
  agent.jobs.push(Job{2, job});
}

bool ActionSystem::m_has_qualities_required(const std::vector<std::string>& qualities_required,
                                            const entt::entity entity,
                                            entt::registry& registry)
{
  const auto& carried_items = registry.get<CarriedItems>(entity);
  const auto& weared_items = registry.get<WearedItems>(entity);
  const auto& wielded_items = registry.get<WieldedItems>(entity);

  for (const auto& quality : qualities_required)
  {
    bool has_quality = false;

    for (const auto item_entity : carried_items.items)
    {
      const auto& item = registry.get<Item>(item_entity);
      const auto& item_data = m_world.get_item_data(item.id);

      if (item_data.qualities.contains(quality))
      {
        has_quality = true;
        continue;
      }
    }

    if (!has_quality)
    {
      for (const auto item_entity : weared_items.items)
      {
        const auto& item = registry.get<Item>(item_entity);
        const auto& item_data = m_world.get_item_data(item.id);

        if (item_data.qualities.contains(quality))
        {
          has_quality = true;
          continue;
        }
      }
    }

    if (!has_quality)
    {
      if (registry.valid(wielded_items.left_hand))
      {
        const auto& item = registry.get<Item>(wielded_items.left_hand);
        const auto& item_data = m_world.get_item_data(item.id);

        if (item_data.qualities.contains(quality))
        {
          has_quality = true;
        }
      }

      if (!has_quality && registry.valid(wielded_items.right_hand))
      {
        const auto& item = registry.get<Item>(wielded_items.right_hand);
        const auto& item_data = m_world.get_item_data(item.id);

        if (item_data.qualities.contains(quality))
        {
          has_quality = true;
        }
      }
    }

    if (!has_quality)
    {
      return false;
    }
  }
  return true;
}

bool ActionSystem::m_has_consumables(const std::map<uint32_t, uint32_t>& consumables, entt::registry& registry)
{
  // TODO: Check society inventory for consumable items
  (void)consumables;
  (void)registry;
  return true;
}

std::vector<entt::entity> ActionSystem::m_select_available_entities(entt::registry& registry)
{
  std::vector<entt::entity> entities;

  for (const auto entity : registry.view<SocietyAgent, Selectable>())
  {
    const auto& agent = registry.get<SocietyAgent>(entity);

    if (!agent.jobs.empty())
    {
      continue;
    }

    entities.push_back(entity);
  }

  return entities;
}

}  // namespace dl
