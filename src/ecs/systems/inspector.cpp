#include "./inspector.hpp"

#include <spdlog/spdlog.h>

#include "ecs/components/biology.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/rectangle.hpp"
#include "ecs/components/selectable.hpp"
#include "ecs/components/velocity.hpp"
#include "graphics/camera.hpp"
#include "world/world.hpp"

namespace dl
{
InspectorSystem::InspectorSystem(World& world) : m_world(world) {}

void InspectorSystem::update(entt::registry& registry, const Camera& camera)
{
  const auto mouse_position = m_input_manager->get_mouse_position();
  const auto& camera_position = camera.get_position();

  // The mouse didn't change the position, nothing changed, don't update
  if (mouse_position == m_last_mouse_position && camera_position.x == m_last_camera_position.first &&
      camera_position.y == m_last_camera_position.second)
  {
    return;
  }

  const auto tile_x = (mouse_position.first + camera_position.x) / 32.f;
  const auto tile_y = (mouse_position.second + camera_position.y) / 32.f;

  const auto entity = m_world.spatial_hash.search_by_component<Selectable>(tile_x, tile_y, registry);

  if (registry.valid(entity))
  {
    m_update_inspector_content(entity, registry);
  }
  else if (registry.valid(m_inspector_entity))
  {
    m_destroy_inspector(registry);
  }

  m_last_mouse_position = mouse_position;
  m_last_camera_position.first = camera_position.x;
  m_last_camera_position.second = camera_position.y;
}

void InspectorSystem::m_update_inspector_content(const entt::entity entity, entt::registry& registry)
{
  if (!registry.valid(m_inspector_entity))
  {
    m_inspector_entity = registry.create();
  }

  if (registry.all_of<Rectangle, Position>(m_inspector_entity))
  {
  }
  else
  {
    registry.emplace<Rectangle>(m_inspector_entity, 100, 100, "#ffffffff");
    registry.emplace<Position>(m_inspector_entity, 30.0, 30.0, 10.0);
  }
}

void InspectorSystem::m_destroy_inspector(entt::registry& registry) { registry.destroy(m_inspector_entity); }

}  // namespace dl
