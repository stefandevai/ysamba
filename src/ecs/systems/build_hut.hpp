#pragma once

#include <entt/entity/registry.hpp>

#include "core/input_manager.hpp"

namespace dl::ui
{
class UIManager;
class Notification;
}  // namespace dl::ui

namespace dl
{
class World;
struct Position;
struct EventEmitter;
class Camera;

class BuildHutSystem
{
 public:
  BuildHutSystem(World& world, EventEmitter& event_emitter, ui::UIManager& ui_manager);

  void update(entt::registry& registry);
  void update_state(entt::registry& registry, const Camera& camera);

 private:
  enum class State
  {
    None,
    SelectHutTarget,
  };

  State m_state = State::None;
  World& m_world;
  InputManager& m_input_manager = InputManager::get_instance();
  ui::UIManager& m_ui_manager;
  ui::Notification* m_notification = nullptr;

  void m_random_walk(entt::registry& registry, const entt::entity entity, const entt::entity job);
  bool m_is_within_hut_bounds(const Position& agent_position, const Position& hut_position, const uint32_t hut_size);
  void m_update_select_target(entt::registry& registry, const Camera& camera);
  bool m_can_build_hut(const uint32_t hut_size, const Vector3i& position);
  void m_select_hut_target(entt::registry& registry, const Camera& camera);
  void m_preview_hut_target(const Vector3i& tile_position, const uint32_t hut_size, entt::registry& registry);
  void m_create_hut_job(const Vector3i& tile_position, const uint32_t hut_size, entt::registry& registry);
  std::vector<entt::entity> m_select_available_entities(entt::registry& registry);
  void m_dispose();
};
}  // namespace dl
