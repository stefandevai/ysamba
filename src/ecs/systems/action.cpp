#include "./action.hpp"

#include <spdlog/spdlog.h>

#include <entt/core/hashed_string.hpp>

#include "ecs/components/action_pickup.hpp"
#include "ecs/components/carried_items.hpp"
#include "ecs/components/item.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/rectangle.hpp"
#include "ecs/components/selectable.hpp"
#include "ecs/components/society_agent.hpp"
#include "graphics/camera.hpp"
#include "graphics/text.hpp"
#include "ui/components/action_menu.hpp"
#include "ui/components/label.hpp"
#include "ui/ui_manager.hpp"
#include "world/world.hpp"

namespace dl
{
const ui::ItemList ActionSystem::m_menu_items = {
    {0, "[H]arvest"},
    {1, "[B]reak"},
    {2, "[D]ig"},
};

ActionSystem::ActionSystem(World& world, ui::UIManager& ui_manager) : m_world(world), m_ui_manager(ui_manager)
{
  m_action_menu = std::make_shared<ui::ActionMenu>(m_menu_items, m_on_select_generic_action);

  m_select_target_label = std::make_shared<ui::Label>("Select target");
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

  if (m_action_menu_id < 0)
  {
    m_open_action_menu();
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
    if (m_selected_entities.empty())
    {
      return;
    }

    m_action_menu->set_actions(m_menu_items);
    m_input_manager.push_context("action_menu"_hs);
    m_state = ActionMenuState::Open;
  }
  else if (m_input_manager.has_clicked(InputManager::MouseButton::Left))
  {
    const auto& mouse_position = m_input_manager.get_mouse_tile_position(camera);

    const auto selected_entity =
        m_world.spatial_hash.get_by_component<Selectable>(mouse_position.x, mouse_position.y, registry);

    // If a selectable entity was clicked, toggle its selected state
    if (registry.valid(selected_entity))
    {
      auto& selectable = registry.get<Selectable>(selected_entity);
      selectable.selected = !selectable.selected;

      if (selectable.selected)
      {
        registry.emplace<Rectangle>(selected_entity, 32, 32, 0x5588cc88);
        m_selected_entities.push_back(selected_entity);
      }
      else
      {
        if (registry.all_of<Rectangle>(selected_entity))
        {
          registry.remove<Rectangle>(selected_entity);
        }
        m_selected_entities.erase(std::find(m_selected_entities.begin(), m_selected_entities.end(), selected_entity));
      }
    }
    // If we are not selecting an entity, walk to the target
    else if (!m_selected_entities.empty())
    {
      for (const auto entity : m_selected_entities)
      {
        auto& agent = registry.get<SocietyAgent>(entity);
        agent.jobs.push(Job{JobType::Walk, 0, Target{Vector3i{mouse_position.x, mouse_position.y, 0}, 0, 0}});
      }
    }
  }
  else if (m_input_manager.has_clicked(InputManager::MouseButton::Right))
  {
    if (m_selected_entities.empty())
    {
      return;
    }

    const auto& mouse_position = m_input_manager.get_mouse_tile_position(camera);
    const auto selected_entity =
        m_world.spatial_hash.get_by_component<Item>(mouse_position.x, mouse_position.y, registry);

    if (registry.valid(selected_entity))
    {
      // Get item actions
      const auto& item = registry.get<Item>(selected_entity);
      const auto& item_data = m_world.get_item_data(item.id);
      m_actions.clear();

      if (item_data.flags.contains("PICKABLE") && m_selected_entities.size() == 1)
      {
        m_actions.push_back({static_cast<uint32_t>(JobType::Pickup), "pickup"});
      }

      m_action_menu->set_actions(m_actions);
      m_action_menu->set_on_select([this, &registry, mouse_position, selected_entity](const uint32_t i) {
        for (const auto entity : m_selected_entities)
        {
          m_create_job(
              static_cast<JobType>(i), static_cast<uint32_t>(selected_entity), mouse_position, registry, entity);
        }
        m_dispose();
      });
      m_input_manager.push_context("action_menu"_hs);
      m_state = ActionMenuState::Open;
    }
    else
    {
      // Get tile actions
      const auto& tile_data = m_world.get(mouse_position.x, mouse_position.y, 0);
      m_actions.clear();

      for (const auto& action : tile_data.actions)
      {
        m_actions.push_back({static_cast<uint32_t>(action.first), action.second.name});
      }

      m_action_menu->set_actions(m_actions);
      m_action_menu->set_on_select([this, &registry, mouse_position, &tile_data](const uint32_t i) {
        for (const auto entity : m_selected_entities)
        {
          m_create_job(static_cast<JobType>(i), tile_data.id, mouse_position, registry, entity);
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

  if (m_action_menu_id > -1)
  {
    m_close_action_menu();
  }
  if (m_select_target_label_id < 0)
  {
    m_show_select_target_text();
  }

  if (m_input_manager.poll_action("close_menu"_hs))
  {
    m_dispose();
  }
  else if (m_input_manager.has_clicked(InputManager::MouseButton::Left))
  {
    const auto& mouse_position = m_input_manager.get_mouse_position();
    const auto& camera_position = camera.get_position();
    const auto& tile_size = camera.get_tile_size();

    Vector2i tile_position{};
    tile_position.x = (mouse_position.x + camera_position.x) / tile_size.x;
    tile_position.y = (mouse_position.y + camera_position.y) / tile_size.y;

    switch (m_state)
    {
    case ActionMenuState::SelectHarvestTarget:
    {
      m_select_tile_target(tile_position, JobType::Harvest, registry);
      break;
    }
    case ActionMenuState::SelectBreakTarget:
    {
      m_select_tile_target(tile_position, JobType::Break, registry);
      break;
    }
    case ActionMenuState::SelectDigTarget:
    {
      m_select_tile_target(tile_position, JobType::Dig, registry);
      break;
    }
    default:
    {
      m_dispose();
      break;
    }
    }
  }
}

void ActionSystem::m_open_action_menu()
{
  assert(m_action_menu != nullptr && m_action_menu_id < 0);
  m_action_menu_id = m_ui_manager.add_component(m_action_menu);
}

void ActionSystem::m_show_select_target_text()
{
  assert(m_select_target_label != nullptr && m_select_target_label_id < 0);
  m_select_target_label_id = m_ui_manager.add_component(m_select_target_label);
}

void ActionSystem::m_close_action_menu()
{
  if (m_action_menu_id >= 0)
  {
    m_ui_manager.remove_component(m_action_menu_id);
    m_action_menu_id = -1;
  }
}

void ActionSystem::m_close_select_target()
{
  if (m_select_target_label_id >= 0)
  {
    m_ui_manager.remove_component(m_select_target_label_id);
    m_select_target_label_id = -1;
  }
}

void ActionSystem::m_dispose()
{
  m_close_action_menu();
  m_close_select_target();

  m_state = ActionMenuState::Closed;
  m_input_manager.pop_context();
}

void ActionSystem::m_select_tile_target(const Vector2i& tile_position, const JobType job_type, entt::registry& registry)
{
  const auto& tile = m_world.get(tile_position.x, tile_position.y, 0);

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

      m_create_job(job_type, tile.id, tile_position, registry, entity);
    }
    m_dispose();
  }
}

void ActionSystem::m_select_item_target(const Vector2i& tile_position, const JobType job_type, entt::registry& registry)
{
  const auto item = m_world.spatial_hash.get_by_component<Item>(tile_position.x, tile_position.y, registry);

  if (!registry.valid(item))
  {
    return;
  }

  for (const auto entity : m_selected_entities)
  {
    m_create_job(job_type, static_cast<uint32_t>(item), tile_position, registry, entity);
  }

  m_dispose();
}

void ActionSystem::m_create_job(
    const JobType job_type, const uint32_t id, const Vector2i& position, entt::registry& registry, entt::entity entity)
{
  auto& agent = registry.get<SocietyAgent>(entity);
  agent.jobs.push(Job{JobType::Walk, 2, Target{Vector3i{position.x, position.y, 0}}});
  agent.jobs.push(Job{job_type, 2, Target{Vector3i{position.x, position.y, 0}, id}});
}

bool ActionSystem::m_has_qualities_required(const std::vector<std::string>& qualities_required,
                                            const entt::entity entity,
                                            entt::registry& registry)
{
  for (const auto& quality : qualities_required)
  {
    bool has_quality = false;

    const auto& carried_items = registry.get<CarriedItems>(entity);

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
      return false;
    }
  }
  return true;
}

bool ActionSystem::m_has_consumables(const std::map<uint32_t, uint32_t>& consumables, entt::registry& registry)
{
  // TODO: Check society inventory for consumable items
  return true;
}

}  // namespace dl
