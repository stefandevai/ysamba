#pragma once

#include <entt/entity/registry.hpp>

#include "core/input_manager.hpp"

namespace dl
{
class World;

class BuildHutSystem
{
 public:
  BuildHutSystem(World& world);

  void update(entt::registry& registry);

 private:
  World& m_world;
};
}  // namespace dl
