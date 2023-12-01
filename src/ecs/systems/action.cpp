#include "./action.hpp"

#include <spdlog/spdlog.h>

#include "ecs/components/action_break.hpp"
#include "ecs/components/action_harvest.hpp"
#include "ecs/components/action_pickup.hpp"
#include "ecs/components/biology.hpp"
#include "ecs/components/pickable.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/rectangle.hpp"
#include "ecs/components/selectable.hpp"
#include "ecs/components/society_agent.hpp"
#include "ecs/components/velocity.hpp"
#include "graphics/camera.hpp"
#include "graphics/text.hpp"
#include "ui/components/action_menu.hpp"
#include "ui/components/label.hpp"
#include "ui/ui_manager.hpp"
#include "world/item_target.hpp"
#include "world/tile_flag.hpp"
#include "world/tile_target.hpp"
#include "world/world.hpp"

namespace dl
{
const std::vector<std::string> ActionSystem::menu_items = {
    "[H]arvest",
    "[P]ickup",
    "[B]reak",
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
    m_update_action_menu(registry);
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

void ActionSystem::m_update_action_menu(entt::registry& registry)
{
  if (m_action_menu_id < 0)
  {
    m_open_action_menu();
  }

  if (m_input_manager->poll_action("close_menu"))
  {
    m_dispose(registry);
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
}

void ActionSystem::m_update_closed_menu(entt::registry& registry, const Camera& camera)
{
  if (m_input_manager->has_clicked(InputManager::MouseButton::Left))
  {
    const auto& mouse_position = m_input_manager->get_mouse_position();
    const auto& camera_position = camera.get_position();
    const auto& tile_size = camera.get_tile_size();

    const auto tile_x = (mouse_position.x + camera_position.x) / tile_size.x;
    const auto tile_y = (mouse_position.y + camera_position.y) / tile_size.y;

    const auto entity = m_world.spatial_hash.get_by_component<Selectable>(tile_x, tile_y, registry);

    if (registry.valid(entity))
    {
      auto& selectable = registry.get<Selectable>(entity);
      selectable.selected = true;
      m_selected_entities.push_back(entity);

      m_state = ActionMenuState::Open;
      m_input_manager->push_context("action_menu");
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
    m_dispose(registry);
  }
  else if (m_input_manager->has_clicked(InputManager::MouseButton::Left))
  {
    const auto& mouse_position = m_input_manager->get_mouse_position();
    const auto& camera_position = camera.get_position();
    const auto& tile_size = camera.get_tile_size();

    const auto tile_x = (mouse_position.x + camera_position.x) / tile_size.x;
    const auto tile_y = (mouse_position.y + camera_position.y) / tile_size.y;

    switch (m_state)
    {
    case ActionMenuState::SelectHarvestTarget:
    {
      m_select_harvest_target(tile_x, tile_y, registry);
      break;
    }
    case ActionMenuState::SelectPickupTarget:
    {
      m_select_pickup_target(tile_x, tile_y, registry);
      break;
    }
    case ActionMenuState::SelectBreakTarget:
    {
      m_select_break_target(tile_x, tile_y, registry);
      break;
    }
    default:
    {
      m_dispose(registry);
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

void ActionSystem::m_dispose(entt::registry& registry)
{
  m_close_action_menu();
  m_close_select_target();

  for (const auto entity : m_selected_entities)
  {
    auto& selectable = registry.get<Selectable>(entity);
    selectable.selected = false;
  }

  m_selected_entities.clear();
  m_state = ActionMenuState::Closed;
  m_input_manager->pop_context();
}

void ActionSystem::m_select_harvest_target(const int tile_x, const int tile_y, entt::registry& registry)
{
  const auto& tile = m_world.get(tile_x, tile_y, 0);

  if (tile.flags.contains(tile_flag::harvestable))
  {
    for (const auto entity : m_selected_entities)
    {
      const auto& position = registry.get<Position>(entity);
      const Vector3i tile_position = {
          static_cast<int>(position.x), static_cast<int>(position.y), static_cast<int>(position.x)};
      auto& action_harvest =
          registry.emplace_or_replace<ActionHarvest>(entity, TileTarget(tile.id, tile_x, tile_y, position.z));
      action_harvest.target.path = m_world.get_path_between(tile_position, Vector3i{tile_x, tile_y, (int)position.z});
    }
    m_dispose(registry);
  }
}

void ActionSystem::m_select_pickup_target(const int tile_x, const int tile_y, entt::registry& registry)
{
  const auto item = m_world.spatial_hash.get_by_component<Pickable>(tile_x, tile_y, registry);

  if (!registry.valid(item))
  {
    return;
  }

  // TODO: Move whole pickup action to an idividual menu, multiple people can't take a
  // single item in this stage of development
  for (const auto entity : m_selected_entities)
  {
    const auto& position = registry.get<Position>(entity);
    const Vector3i tile_position = {
        static_cast<int>(position.x), static_cast<int>(position.y), static_cast<int>(position.x)};
    auto& action_pickup =
        registry.emplace_or_replace<ActionPickup>(entity, ItemTarget(item, tile_x, tile_y, position.z));
    action_pickup.target.path = m_world.get_path_between(tile_position, Vector3i{tile_x, tile_y, (int)position.z});
  }

  m_dispose(registry);
}

void ActionSystem::m_select_break_target(const int tile_x, const int tile_y, entt::registry& registry)
{
  const auto& tile = m_world.get(tile_x, tile_y, 0);

  if (tile.flags.contains(tile_flag::breakable))
  {
    for (const auto entity : m_selected_entities)
    {
      const auto& position = registry.get<Position>(entity);
      const Vector3i tile_position = {
          static_cast<int>(position.x), static_cast<int>(position.y), static_cast<int>(position.x)};
      auto& action_break =
          registry.emplace_or_replace<ActionBreak>(entity, TileTarget(tile.id, tile_x, tile_y, position.z));
      action_break.target.path = m_world.get_path_between(tile_position, Vector3i{tile_x, tile_y, (int)position.z});
    }

    m_dispose(registry);
  }
}

}  // namespace dl
