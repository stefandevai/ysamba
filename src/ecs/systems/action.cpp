#include "./action.hpp"

#include <spdlog/spdlog.h>

#include "ecs/components/action_pickup.hpp"
#include "ecs/components/pickable.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/rectangle.hpp"
#include "ecs/components/selectable.hpp"
#include "ecs/components/society_agent.hpp"
#include "graphics/camera.hpp"
#include "graphics/text.hpp"
#include "ui/components/action_menu.hpp"
#include "ui/components/label.hpp"
#include "ui/ui_manager.hpp"
#include "world/item_target.hpp"
#include "world/tile_target.hpp"
#include "world/world.hpp"

namespace dl
{
const std::vector<std::string> ActionSystem::menu_items = {
    "[H]arvest",
    "[P]ickup",
    "[B]reak",
    "[D]ig",
};

ActionSystem::ActionSystem(World& world, ui::UIManager& ui_manager) : m_world(world), m_ui_manager(ui_manager)
{
  auto on_select = [this](const int i) { m_state = static_cast<ActionMenuState>(i); };

  m_action_menu = std::make_shared<ui::ActionMenu>(menu_items, on_select);

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
  if (m_action_menu_id < 0)
  {
    m_open_action_menu();
  }

  if (m_input_manager->poll_action("close_menu"))
  {
    m_dispose();
  }
  else if (m_input_manager->poll_action("harvest"))
  {
    m_state = ActionMenuState::SelectHarvestTarget;
  }
  else if (m_input_manager->poll_action("pickup"))
  {
    m_state = ActionMenuState::SelectPickupTarget;
  }
  else if (m_input_manager->poll_action("break"))
  {
    m_state = ActionMenuState::SelectBreakTarget;
  }
  else if (m_input_manager->poll_action("dig"))
  {
    m_state = ActionMenuState::SelectDigTarget;
  }
}

void ActionSystem::m_update_closed_menu(entt::registry& registry, const Camera& camera)
{
  if (m_input_manager->poll_action("open_action_menu"))
  {
    if (m_selected_entities.empty())
    {
      return;
    }

    m_state = ActionMenuState::Open;
    m_input_manager->push_context("action_menu");
  }
  else if (m_input_manager->has_clicked(InputManager::MouseButton::Left))
  {
    const auto& mouse_position = m_input_manager->get_mouse_position();
    const auto& camera_position = camera.get_position();
    const auto& tile_size = camera.get_tile_size();

    const int tile_x = (mouse_position.x + camera_position.x) / tile_size.x;
    const int tile_y = (mouse_position.y + camera_position.y) / tile_size.y;

    const auto selected_entity = m_world.spatial_hash.get_by_component<Selectable>(tile_x, tile_y, registry);

    // If a selectable entity was clicked, toggle its selected state
    if (registry.valid(selected_entity))
    {
      auto& selectable = registry.get<Selectable>(selected_entity);
      selectable.selected = !selectable.selected;

      if (selectable.selected)
      {
        registry.emplace<Rectangle>(selected_entity, 32, 32, "#5588cc88");
        m_selected_entities.push_back(selected_entity);
      }
      else
      {
        registry.remove<Rectangle>(selected_entity);
        m_selected_entities.erase(std::find(m_selected_entities.begin(), m_selected_entities.end(), selected_entity));
      }
    }
    // If we are not selecting an entity, walk to the target
    else if (!m_selected_entities.empty())
    {
      for (const auto entity : m_selected_entities)
      {
        auto& agent = registry.get<SocietyAgent>(entity);
        agent.jobs.push(Job{JobType::Walk, 0, Target{Vector3i{tile_x, tile_y, 0}}});
      }
    }
  }
}

void ActionSystem::m_update_selecting_target(entt::registry& registry, const Camera& camera)
{
  if (m_action_menu_id > -1)
  {
    m_close_action_menu();
  }
  if (m_select_target_label_id < 0)
  {
    m_show_select_target_text();
  }

  if (m_input_manager->poll_action("close_menu"))
  {
    m_dispose();
  }
  else if (m_input_manager->has_clicked(InputManager::MouseButton::Left))
  {
    const auto& mouse_position = m_input_manager->get_mouse_position();
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
    case ActionMenuState::SelectPickupTarget:
    {
      m_select_item_target(tile_position, JobType::Pickup, registry);
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
  m_input_manager->pop_context();
}

void ActionSystem::m_select_tile_target(const Vector2i& tile_position, const JobType job_type, entt::registry& registry)
{
  const auto& tile = m_world.get(tile_position.x, tile_position.y, 0);

  if (tile.actions.contains(job_type))
  {
    for (const auto entity : m_selected_entities)
    {
      auto& agent = registry.get<SocietyAgent>(entity);
      agent.jobs.push(Job{JobType::Walk, 2, Target{Vector3i{tile_position.x, tile_position.y, 0}}});
      agent.jobs.push(Job{job_type, 2, Target{Vector3i{tile_position.x, tile_position.y, 0}, tile.id}});
    }
    m_dispose();
  }
}

void ActionSystem::m_select_item_target(const Vector2i& tile_position, const JobType job_type, entt::registry& registry)
{
  const auto item = m_world.spatial_hash.get_by_component<Pickable>(tile_position.x, tile_position.y, registry);

  if (!registry.valid(item))
  {
    return;
  }

  // TODO: Move whole pickup action to an idividual menu, multiple people can't take a
  // single item in this stage of development
  for (const auto entity : m_selected_entities)
  {
    auto& agent = registry.get<SocietyAgent>(entity);
    agent.jobs.push(Job{JobType::Walk, 2, Target{Vector3i{tile_position.x, tile_position.y, 0}}});
    agent.jobs.push(
        Job{job_type, 2, Target{Vector3i{tile_position.x, tile_position.y, 0}, static_cast<uint32_t>(item)}});
  }

  m_dispose();
}

}  // namespace dl
