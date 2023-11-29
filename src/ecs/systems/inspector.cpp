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
#include "ui/components/inspector.hpp"
#include "ui/ui_manager.hpp"
#include "world/world.hpp"

namespace dl
{
InspectorSystem::InspectorSystem(World& world, ui::UIManager& ui_manager) : m_world(world), m_ui_manager(ui_manager)
{
  m_inspector = std::make_shared<ui::Inspector>();
}

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
    m_update_inspector_content(entity, registry);
  }
  else if (m_is_valid())
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

}  // namespace dl
