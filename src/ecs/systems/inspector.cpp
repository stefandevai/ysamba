#include "./inspector.hpp"

#include <spdlog/spdlog.h>

#include "ecs/components/biology.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/rectangle.hpp"
#include "ecs/components/selectable.hpp"
#include "ecs/components/society_agent.hpp"
#include "ecs/components/velocity.hpp"
#include "graphics/camera.hpp"
#include "graphics/text.hpp"
#include "world/world.hpp"

namespace dl
{
InspectorSystem::InspectorSystem(World& world) : m_world(world) {}

void InspectorSystem::update(entt::registry& registry, const Camera& camera)
{
  const auto mouse_position = m_input_manager->get_mouse_position();
  const auto& camera_position = camera.get_position();

  // The mouse didn't change the position, nothing changed, don't update
  if (mouse_position == m_last_mouse_position && camera_position == m_last_camera_position)
  {
    return;
  }

  const auto& tile_size = camera.get_tile_size();
  const auto tile_x = (mouse_position.x + camera_position.x) / tile_size.x;
  const auto tile_y = (mouse_position.y + camera_position.y) / tile_size.y;

  const auto entity = m_world.spatial_hash.get_by_component<Selectable>(tile_x, tile_y, registry);

  if (registry.valid(entity))
  {
    m_update_inspector_content(entity, registry, camera);
  }
  else if (m_is_valid(registry))
  {
    m_destroy_inspector(registry);
  }

  m_last_mouse_position = mouse_position;
  m_last_camera_position = camera_position;
}

void InspectorSystem::m_update_inspector_content(const entt::entity entity,
                                                 entt::registry& registry,
                                                 const Camera& camera)
{
  if (!m_is_valid(registry))
  {
    m_inspector_quad = registry.create();
    m_inspector_text = registry.create();
  }

  const auto& camera_size = camera.get_size();

  if (!registry.all_of<Rectangle, Position>(m_inspector_quad))
  {
    registry.emplace<Rectangle>(m_inspector_quad, 300, 100, "#1b2420aa");
    registry.emplace<Position>(m_inspector_quad, camera_size.x - 30.0 - 300.0, 30.0, 10.0);
  }
  if (!registry.all_of<Text, Position>(m_inspector_text))
  {
    registry.emplace<Text>(m_inspector_text, "");
    registry.emplace<Position>(m_inspector_text, camera_size.x - 15.0 - 300.0, 45.0, 10.0);
  }

  if (registry.all_of<SocietyAgent>(entity))
  {
    const auto& agent = registry.get<SocietyAgent>(entity);

    auto& text = registry.get<Text>(m_inspector_text);
    text.set_text(agent.name);

    /* const auto& position = registry.get<Position>(entity); */
    /* text.set_text(agent.name + " (" + std::to_string(position.x) + ", " + std::to_string(position.y) + ")"); */
  }
}

void InspectorSystem::m_destroy_inspector(entt::registry& registry)
{
  registry.destroy(m_inspector_quad);
  registry.destroy(m_inspector_text);
}

bool InspectorSystem::m_is_valid(const entt::registry& registry) const
{
  return registry.valid(m_inspector_quad) && registry.valid(m_inspector_text);
}

}  // namespace dl
