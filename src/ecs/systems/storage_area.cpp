#include "./storage_area.hpp"

#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include "core/events/action.hpp"
#include "core/events/emitter.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/storage_area.hpp"
#include "graphics/camera.hpp"
#include "ui/compositions/notification.hpp"
#include "ui/ui_manager.hpp"
#include "world/world.hpp"

namespace dl
{
StorageAreaSystem::StorageAreaSystem(World& world, EventEmitter& event_emitter, ui::UIManager& ui_manager)
    : m_world(world), m_ui_manager(ui_manager)
{
  event_emitter.on<SelectStorageEvent>([this](const SelectStorageEvent& event, EventEmitter& emitter) {
    (void)emitter;
    (void)event;
    m_state = State::SelectArea;
  });
}

void StorageAreaSystem::update(entt::registry& registry) { using namespace entt::literals; }

void StorageAreaSystem::update_state(entt::registry& registry, const Camera& camera)
{
  switch (m_state)
  {
  case State::SelectArea:
  {
    m_update_select_area(registry, camera);
    break;
  }
  default:
    break;
  }
}

void StorageAreaSystem::m_update_select_area(entt::registry& registry, const Camera& camera)
{
  using namespace entt::literals;

  if (!m_input_manager.is_context("action_menu"_hs))
  {
    m_input_manager.push_context("action_menu"_hs);
  }
  if (m_input_manager.poll_action("close_menu"_hs))
  {
    m_dispose();
    return;
  }

  if (m_notification == nullptr)
  {
    m_notification = m_ui_manager.notify("Select storage area");
  }

  m_select_area(registry, camera);
}

void StorageAreaSystem::m_select_area(entt::registry& registry, const Camera& camera)
{
  const auto& drag_bounds = m_input_manager.get_drag_bounds();

  Vector3i begin = m_world.screen_to_world(Vector2i{drag_bounds.x, drag_bounds.y}, camera);
  Vector3i end = m_world.screen_to_world(Vector2i{drag_bounds.z, drag_bounds.w}, camera);

  const Vector3i iteration_begin{std::min(begin.x, end.x), std::min(begin.y, end.y), begin.z};

  const Vector3i iteration_end{std::max(begin.x, end.x), std::max(begin.y, end.y), begin.z};

  if (m_input_manager.is_dragging())
  {
    if (iteration_begin == m_last_begin && iteration_end == m_last_end)
    {
      return;
    }

    m_last_begin = iteration_begin;
    m_last_end = iteration_end;

    m_preview_area(iteration_begin, iteration_end, registry);
  }
  else if (m_input_manager.has_dragged())
  {
    m_last_begin = Vector3i{};
    m_last_end = Vector3i{};

    m_set_storage_area(iteration_begin, iteration_end, registry);
  }
}

void StorageAreaSystem::m_preview_area(const Vector3i& begin, const Vector3i& end, entt::registry& registry)
{
  using namespace entt::literals;

  assert(begin.x <= end.x && begin.y <= end.y);

  for (const auto entity : registry.view<entt::tag<"storage_preview"_hs>>())
  {
    registry.destroy(entity);
  }

  const auto texture_id = m_world.get_spritesheet_id();

  Color preview_tile_color{0x66EEAA77};

  if (!m_is_area_valid(begin, end))
  {
    preview_tile_color = Color{0xEE664477};
  }

  for (int j = begin.y; j <= end.y; ++j)
  {
    for (int i = begin.x; i <= end.x; ++i)
    {
      const auto entity = registry.create();
      const auto sprite = Sprite{
          .id = 2,
          .resource_id = texture_id,
          .layer_z = 4,
          .category = "tile",
          .color = preview_tile_color,
      };

      registry.emplace<Sprite>(entity, sprite);
      registry.emplace<Position>(entity, i, j, begin.z);
      registry.emplace<entt::tag<"storage_preview"_hs>>(entity);
    }
  }
}

void StorageAreaSystem::m_set_storage_area(const Vector3i& begin, const Vector3i& end, entt::registry& registry)
{
  using namespace entt::literals;

  assert(begin.x <= end.x && begin.y <= end.y);

  if (!m_is_area_valid(begin, end))
  {
    for (const auto entity : registry.view<entt::tag<"storage_preview"_hs>>())
    {
      registry.destroy(entity);
    }
    return;
  }

  const auto texture_id = m_world.get_spritesheet_id();

  for (int j = begin.y; j <= end.y; ++j)
  {
    for (int i = begin.x; i <= end.x; ++i)
    {
      const auto entity = registry.create();
      const auto sprite = Sprite{
          .id = 2,
          .resource_id = texture_id,
          .layer_z = 4,
          .category = "tile",
          .color = Color{0x66EEAA77},
      };

      registry.emplace<StorageArea>(entity);
      registry.emplace<Sprite>(entity, sprite);
      registry.emplace<Position>(entity, i, j, begin.z);
    }
  }

  m_dispose();
}

bool StorageAreaSystem::m_is_area_valid(const Vector3i& begin, const Vector3i& end)
{
  assert(begin.x <= end.x && begin.y <= end.y);

  for (int j = begin.y; j <= end.y; ++j)
  {
    for (int i = begin.x; i <= end.x; ++i)
    {
      if (!m_world.is_walkable(i, j, begin.z))
      {
        return false;
      }
    }
  }

  return true;
}

void StorageAreaSystem::m_dispose()
{
  m_input_manager.pop_context();

  if (m_notification != nullptr)
  {
    m_notification->hide();
    m_notification = nullptr;
  }

  m_state = State::None;
}

}  // namespace dl
