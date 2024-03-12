#pragma once

#include <entt/entity/registry.hpp>

#include "core/input_manager.hpp"

namespace dl
{
class World;
struct Position;

class BuildHutSystem
{
 public:
  BuildHutSystem(World& world);

  void update(entt::registry& registry);

 private:
  World& m_world;

  void m_random_walk(entt::registry& registry, const entt::entity entity, const entt::entity job);
  bool m_is_within_hut_bounds(const Position& agent_position, const Position& hut_position, const uint32_t hut_size);
};
}  // namespace dl
