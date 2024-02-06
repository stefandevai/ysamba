#pragma once

#include <entt/entity/registry.hpp>

#include "core/input_manager.hpp"

namespace dl
{
class World;
struct EventEmitter;

class PlayerControlsSystem
{
 public:
  PlayerControlsSystem(EventEmitter& event_emitter);

  void update(entt::registry& registry, const entt::entity player);

 private:
  InputManager& m_input_manager = InputManager::get_instance();
  EventEmitter& m_event_emitter;
};
}  // namespace dl
