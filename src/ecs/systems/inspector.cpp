#include "./inspector.hpp"

#include <spdlog/spdlog.h>

#include <entt/core/hashed_string.hpp>

#include "ecs/components/item.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/rectangle.hpp"
#include "ecs/components/selectable.hpp"
#include "ecs/components/society_agent.hpp"
#include "ecs/components/visibility.hpp"
#include "graphics/camera.hpp"
#include "ui/components/inspector.hpp"
#include "ui/ui_manager.hpp"
#include "world/tile_data.hpp"
#include "world/world.hpp"

namespace dl
{
InspectorSystem::InspectorSystem(World& world, ui::UIManager& ui_manager) : m_world(world), m_ui_manager(ui_manager)
{
  m_inspector = std::make_shared<ui::Inspector>();
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

  const auto& tile_size = camera.get_tile_size();
  const auto tile_x = (mouse_position.x + camera_position.x) / tile_size.x;
  const auto tile_y = (mouse_position.y + camera_position.y) / tile_size.y;

  if (!registry.valid(m_target_quad))
  {
    m_target_quad = registry.create();
    registry.emplace<Rectangle>(m_target_quad, tile_size.x, tile_size.y, 0xe3c16488);
    registry.emplace<Position>(m_target_quad, 0, 0, 4);
  }

  auto& quad_position = registry.get<Position>(m_target_quad);
  quad_position.x = std::floor(tile_x);
  quad_position.y = std::floor(tile_y);

  const auto entity = m_world.spatial_hash.get_by_component<Visibility>(tile_x, tile_y, registry);
  bool updated_inspector_content = false;

  if (registry.valid(entity))
  {
    m_update_inspector_content(entity, registry);
    updated_inspector_content = true;
  }
  else
  {
    const auto& tile_data = m_world.get(tile_x, tile_y, 0);

    if (tile_data.id > 0)
    {
      m_update_inspector_content(tile_data);
      updated_inspector_content = true;
    }
  }

  if (!updated_inspector_content && m_is_valid())
  {
    m_destroy_inspector();
  }

  m_last_mouse_position = mouse_position;
  m_last_camera_position = camera_position;
}

void InspectorSystem::m_update_inspector_content(const entt::entity entity, entt::registry& registry)
{
  if (!m_is_valid())
  {
    m_inspector_id = m_ui_manager.add_component(m_inspector);
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
    m_inspector->set_content(item_data.name);
  }
}

void InspectorSystem::m_update_inspector_content(const TileData& tile_data)
{
  if (!m_is_valid())
  {
    m_inspector_id = m_ui_manager.add_component(m_inspector);
  }

  m_inspector->set_content(tile_data.name);
}

void InspectorSystem::m_destroy_inspector()
{
  if (m_inspector_id >= 0)
  {
    m_ui_manager.remove_component(m_inspector_id);
    m_inspector_id = -1;
  }
}

bool InspectorSystem::m_is_valid() const { return m_inspector_id >= 0 && m_inspector != nullptr; }

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
