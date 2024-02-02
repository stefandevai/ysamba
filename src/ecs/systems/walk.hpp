#pragma once

#include <entt/entity/registry.hpp>

#include "core/input_manager.hpp"

namespace dl
{
class World;

class WalkSystem
{
 public:
  WalkSystem(World& world);

  void update(entt::registry& registry);

 private:
  World& m_world;
  InputManager& m_input_manager = InputManager::get_instance();
};
}  // namespace dl
