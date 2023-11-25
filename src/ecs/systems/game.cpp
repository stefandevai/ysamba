#include "./game.hpp"

#include <spdlog/spdlog.h>

#include <entt/entity/registry.hpp>

#include "ecs/components/position.hpp"
#include "world/world.hpp"

namespace dl
{
GameSystem::GameSystem(entt::registry& registry, World& world) : m_world(world)
{
  registry.on_construct<Position>().connect<&GameSystem::m_add_to_spatial_hash>(this);
  registry.on_update<Position>().connect<&GameSystem::m_update_spatial_hash>(this);
  registry.on_destroy<Position>().connect<&GameSystem::m_remove_from_spatial_hash>(this);
}

void GameSystem::update() {}

void GameSystem::m_add_to_spatial_hash(entt::registry& registry, entt::entity entity)
{
  auto& position = registry.get<Position>(entity);
  const auto index = m_world.spatial_hash.add(entity, std::round(position.x), std::round(position.y));
  position.spatial_hash_index = index;
}

void GameSystem::m_update_spatial_hash(entt::registry& registry, entt::entity entity)
{
  auto& position = registry.get<Position>(entity);
  const auto index =
      m_world.spatial_hash.update(entity, std::round(position.x), std::round(position.y), position.spatial_hash_index);
  position.spatial_hash_index = index;
}

void GameSystem::m_remove_from_spatial_hash(entt::registry& registry, entt::entity entity)
{
  auto& position = registry.get<Position>(entity);
  m_world.spatial_hash.remove(entity, position.spatial_hash_index);
}
}  // namespace dl
