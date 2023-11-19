#include "./game.hpp"

#include <spdlog/spdlog.h>
#include "../../world/world.hpp"
#include "../components/position.hpp"

namespace dl
{
  GameSystem::GameSystem(entt::registry& registry, World& world)
    : m_world(world)
  {
    registry.on_construct<Position>().connect<&GameSystem::m_add_to_spatial_hash>(this);
    registry.on_update<Position>().connect<&GameSystem::m_update_spatial_hash>(this);
    registry.on_destroy<Position>().connect<&GameSystem::m_remove_from_spatial_hash>(this);
  }


  void GameSystem::update()
  { }

  void GameSystem::m_add_to_spatial_hash (entt::registry& registry, entt::entity entity)
  {
    auto& position = registry.get<Position> (entity);
    const auto index = m_world.spatial_hash.add(entity, position.x, position.y);
    position.spatial_hash_index = index;
  }

  void GameSystem::m_update_spatial_hash (entt::registry& registry, entt::entity entity)
  {
    auto& position = registry.get<Position> (entity);
    const auto index = m_world.spatial_hash.update(entity, position.x, position.y, position.spatial_hash_index);
    position.spatial_hash_index = index;
  }

  void GameSystem::m_remove_from_spatial_hash (entt::registry& registry, entt::entity entity)
  {
    auto& position = registry.get<Position> (entity);
    m_world.spatial_hash.remove(entity, position.spatial_hash_index);
  }
}

