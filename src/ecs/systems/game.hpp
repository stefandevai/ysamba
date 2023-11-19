#pragma once

#include <entt/entity/fwd.hpp>

namespace dl
{
class World;

class GameSystem
{
 public:
  GameSystem(entt::registry& registry, World& world);

  void update();

 private:
  World& m_world;

  void m_add_to_spatial_hash(entt::registry& registry, entt::entity entity);
  void m_update_spatial_hash(entt::registry& registry, entt::entity entity);
  void m_remove_from_spatial_hash(entt::registry& registry, entt::entity entity);
};
}  // namespace dl
