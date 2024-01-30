#include "./inspector.hpp"

#include <spdlog/spdlog.h>

#include <entt/core/hashed_string.hpp>

#include "constants.hpp"
#include "ecs/components/item.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/rectangle.hpp"
#include "ecs/components/selectable.hpp"
#include "ecs/components/society_agent.hpp"
#include "ecs/components/visibility.hpp"
#include "graphics/camera.hpp"
#include "ui/compositions/inspector.hpp"
#include "ui/ui_manager.hpp"
#include "world/tile_data.hpp"
#include "world/world.hpp"

namespace dl
{
InspectorSystem::InspectorSystem(World& world, ui::UIManager& ui_manager) : m_world(world), m_ui_manager(ui_manager)

{
  m_inspector = m_ui_manager.emplace<ui::Inspector>();
}

void InspectorSystem::update(entt::registry& registry, const Camera& camera)
{
  m_update_input(registry);

  if (m_state != State::Active)
  {
    return;
  }

  const auto mouse_position = m_input_manager.get_mouse_position();
  const auto& camera_position = camera.get_position();

  // The mouse didn't change the position, nothing changed, don't update
  if (mouse_position == m_last_mouse_position && camera_position == m_last_camera_position)
  {
    return;
  }

  const auto& grid_size = camera.get_grid_size();
  const auto tile_x = (mouse_position.x + camera_position.x) / grid_size.x;
  const auto tile_y = (mouse_position.y + camera_position.y) / grid_size.y;
  const int floor_x = std::floor(tile_x);
  const int floor_y = std::floor(tile_y);

  if (!registry.valid(m_target_quad))
  {
    const auto& tile_size = m_world.get_tile_size();
    m_target_quad = registry.create();
    registry.emplace<Rectangle>(m_target_quad, tile_size.x, tile_size.y, 0xe3c16488, 4);
    registry.emplace<Position>(m_target_quad, 0, 0, 0);
  }

  auto& quad_position = registry.get<Position>(m_target_quad);
  quad_position.x = floor_x;
  int elevation = 0;

  // Check elevation in the current mouse position
  for (int z = world::chunk_size.z - 1; z >= 0; --z)
  {
    int queried_elevation = m_world.get_elevation(floor_x, floor_y + z);

    if (queried_elevation == z)
    {
      elevation = queried_elevation;
      break;
    }
  }

  quad_position.y = floor_y + elevation;
  quad_position.z = elevation;

  const auto entity = m_world.spatial_hash.get_by_component<Visibility>(tile_x, tile_y + elevation, registry);
  bool updated_inspector_content = false;

  if (registry.valid(entity))
  {
    const auto& position = registry.get<Position>(entity);

    if (position.z >= elevation)
    {
      m_update_inspector_content(entity, registry);
      updated_inspector_content = true;
    }
  }

  if (!updated_inspector_content)
  {
    const auto& tile_data = m_world.get(tile_x, tile_y + elevation, elevation);

    if (tile_data.id > 0)
    {
      m_update_inspector_content(tile_data);
      updated_inspector_content = true;
    }
  }

  if (!updated_inspector_content && m_inspector->state == ui::UIComponent::State::Visible)
  {
    m_destroy_inspector();
  }

  m_last_mouse_position = mouse_position;
  m_last_camera_position = camera_position;
}

void InspectorSystem::m_update_inspector_content(const entt::entity entity, entt::registry& registry)
{
  if (m_inspector->state == ui::UIComponent::State::Hidden)
  {
    m_inspector->show();
  }

  if (registry.all_of<SocietyAgent>(entity))
  {
    const auto& agent = registry.get<SocietyAgent>(entity);
    m_inspector->set_content(agent.name);
    /* const auto& position = registry.get<Position>(entity); */
    /* text.set_text(agent.name + " (" + std::to_string(position.x) + ", " + std::to_string(position.y) + ")"); */
  }
  else if (registry.all_of<Item, Visibility>(entity))
  {
    const auto& item = registry.get<Item>(entity);
    if (item.id <= 0)
    {
      return;
    }

    const auto& item_data = m_world.get_item_data(item.id);
    m_inspector->set_content(item_data.name + "\nWeight: " + item_data.weight_string +
                             "\nVolume: " + item_data.volume_string);
  }
}

void InspectorSystem::m_update_inspector_content(const TileData& tile_data)
{
  if (m_inspector->state == ui::UIComponent::State::Hidden)
  {
    m_inspector->show();
  }

  m_inspector->set_content(tile_data.name);
}

void InspectorSystem::m_destroy_inspector() { m_inspector->hide(); }

void InspectorSystem::m_update_input(entt::registry& registry)
{
  using namespace entt::literals;

  if (m_state == State::Inactive)
  {
    const auto& current_context = m_input_manager.get_current_context();

    if (current_context == nullptr || current_context->key != "gameplay"_hs)
    {
      return;
    }

    if (m_input_manager.poll_action("inspect"_hs))
    {
      m_state = State::Active;
      m_input_manager.push_context("inspector"_hs);
    }
  }
  else if (m_state == State::Active)
  {
    if (m_input_manager.poll_action("quit"_hs))
    {
      m_destroy_inspector();

      if (registry.valid(m_target_quad))
      {
        registry.destroy(m_target_quad);
      }
      m_state = State::Inactive;
      m_input_manager.pop_context();
    }
  }
}

}  // namespace dl
