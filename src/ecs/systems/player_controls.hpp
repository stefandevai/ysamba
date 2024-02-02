#pragma once

#include <entt/entity/registry.hpp>

#include "core/input_manager.hpp"

namespace dl
{
class World;

class PlayerControlsSystem
{
 public:
  PlayerControlsSystem();

  void update(entt::registry& registry);

 private:
  InputManager& m_input_manager = InputManager::get_instance();
};
}  // namespace dl
