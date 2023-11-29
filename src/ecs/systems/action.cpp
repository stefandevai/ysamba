#include "./action.hpp"

#include <spdlog/spdlog.h>

#include "ecs/components/action_harvest.hpp"
#include "ecs/components/biology.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/rectangle.hpp"
#include "ecs/components/selectable.hpp"
#include "ecs/components/society_agent.hpp"
#include "ecs/components/velocity.hpp"
#include "graphics/camera.hpp"
#include "graphics/text.hpp"
#include "ui/ui_manager.hpp"
#include "world/tile_flag.hpp"
#include "world/tile_target.hpp"
#include "world/world.hpp"

namespace dl
{
ActionSystem::ActionSystem(World& world, ui::UIManager& ui_manager) : m_world(world), m_ui_manager(ui_manager) {}

void ActionSystem::update(entt::registry& registry, const Camera& camera)
{
  if (m_state == ActionMenuState::Open)
  {
    m_update_action_menu(registry, camera);
  }
  else if (m_state == ActionMenuState::SelectingTarget)
  {
    m_update_selecting_target(registry, camera);
  }
  else if (m_state == ActionMenuState::Closed)
  {
    m_update_closed_menu(registry, camera);
  }
}

void ActionSystem::m_update_action_menu(entt::registry& registry, const Camera& camera)
{
  if (!registry.valid(m_menu_quad) || !registry.valid(m_menu_text))
  {
    m_open_action_menu(registry, camera);
  }

  m_open_action_menu(registry, camera);
  if (m_input_manager->poll_action("close_menu"))
  {
    m_close_action_menu(registry);
  }
  else if (m_input_manager->poll_action("harvest"))
  {
    m_state = ActionMenuState::SelectingTarget;
    registry.destroy(m_menu_quad);
    registry.destroy(m_menu_text);
  }
}

void ActionSystem::m_update_closed_menu(entt::registry& registry, const Camera& camera)
{
  if (m_input_manager->is_clicking(InputManager::MouseButton::Left))
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
  if (!registry.valid(m_select_target_text))
  {
    m_show_select_target_text(registry, camera);
  }

  if (m_input_manager->poll_action("close_menu"))
  {
    m_close_action_menu(registry);
  }
  else if (m_input_manager->is_clicking(InputManager::MouseButton::Left))
  {
    const auto& mouse_position = m_input_manager->get_mouse_position();
    const auto& camera_position = camera.get_position();
    const auto& tile_size = camera.get_tile_size();

    const auto tile_x = (mouse_position.x + camera_position.x) / tile_size.x;
    const auto tile_y = (mouse_position.y + camera_position.y) / tile_size.y;

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
        action_harvest.target.path = m_world.get_path_between(tile_position, {tile_x, tile_y, position.z});
      }

      m_close_action_menu(registry);
    }
  }
}

void ActionSystem::m_open_action_menu(entt::registry& registry, const Camera& camera)
{
  if (!registry.valid(m_menu_quad) || !registry.valid(m_menu_text))
  {
    m_menu_quad = registry.create();
    m_menu_text = registry.create();
  }

  const auto& camera_size = camera.get_size();

  if (!registry.all_of<Rectangle, Position>(m_menu_quad))
  {
    registry.emplace<Rectangle>(m_menu_quad, 400, 300, "#1b2420aa");
    registry.emplace<Position>(m_menu_quad, camera_size.x / 2 - 200, camera_size.y / 2 - 150, 10.0);
  }
  if (!registry.all_of<Text, Position>(m_menu_text))
  {
    registry.emplace<Text>(m_menu_text, "Press H to harvest");
    registry.emplace<Position>(m_menu_text, camera_size.x / 2 - 170, camera_size.y / 2 - 120, 10.0);
  }
}

void ActionSystem::m_show_select_target_text(entt::registry& registry, const Camera& camera)
{
  if (!registry.valid(m_select_target_text))
  {
    m_select_target_text = registry.create();
  }

  const auto& camera_size = camera.get_size();

  if (!registry.all_of<Text, Position>(m_select_target_text))
  {
    registry.emplace<Text>(m_select_target_text, "Harvest where?");
    registry.emplace<Position>(m_select_target_text, camera_size.x / 2 - 150, 30, 10.0);
  }
}

void ActionSystem::m_close_action_menu(entt::registry& registry)
{
  if (registry.valid(m_menu_quad))
  {
    registry.destroy(m_menu_quad);
  }
  if (registry.valid(m_menu_text))
  {
    registry.destroy(m_menu_text);
  }
  if (registry.valid(m_select_target_text))
  {
    registry.destroy(m_select_target_text);
  }

  for (const auto entity : m_selected_entities)
  {
    auto& selectable = registry.get<Selectable>(entity);
    selectable.selected = false;
  }

  m_selected_entities.clear();

  m_state = ActionMenuState::Closed;
  m_input_manager->pop_context();
}

}  // namespace dl
