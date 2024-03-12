#include "./game.hpp"

#include <spdlog/spdlog.h>

#include <entt/core/hashed_string.hpp>
#include <entt/entity/registry.hpp>

#include "ecs/components/biology.hpp"
#include "ecs/components/position.hpp"
#include "world/world.hpp"

// TEMP
#include "ecs/components/job_progress.hpp"
// TEMP

namespace dl
{
GameSystem::GameSystem(entt::registry& registry, World& world) : m_world(world)
{
  registry.on_construct<Position>().connect<&GameSystem::m_add_to_spatial_hash>(this);
  registry.on_update<Position>().connect<&GameSystem::m_update_spatial_hash>(this);
  registry.on_destroy<Position>().connect<&GameSystem::m_remove_from_spatial_hash>(this);
}

void GameSystem::update(entt::registry& registry)
{
  auto view = registry.view<Biology>();

  for (const auto entity : view)
  {
    auto& biology = registry.get<Biology>(entity);

    if (biology.energy == 500)
    {
      continue;
    }

    biology.energy += 100;

    if (biology.energy > 500)
    {
      biology.energy = 500;
    }
  }
}

void GameSystem::m_add_to_spatial_hash(entt::registry& registry, entt::entity entity)
{
  using namespace entt::literals;

  if (registry.all_of<entt::tag<"ui"_hs>>(entity))
  {
    return;
  }

  if (registry.all_of<JobProgress>(entity))
  {
    spdlog::debug("AAAAAAAAAAAAAAAAADDING JOB PROGRESS");
  }

  auto& position = registry.get<Position>(entity);
  const auto index
      = m_world.spatial_hash.add(entity, std::round(position.x), std::round(position.y), std::round(position.z));
  position.spatial_hash_index = index;
}

void GameSystem::m_update_spatial_hash(entt::registry& registry, entt::entity entity)
{
  using namespace entt::literals;

  if (registry.all_of<entt::tag<"ui"_hs>>(entity))
  {
    return;
  }

  auto& position = registry.get<Position>(entity);
  const auto index = m_world.spatial_hash.update(
      entity, std::round(position.x), std::round(position.y), std::round(position.z), position.spatial_hash_index);
  position.spatial_hash_index = index;
}

void GameSystem::m_remove_from_spatial_hash(entt::registry& registry, entt::entity entity)
{
  using namespace entt::literals;

  if (registry.all_of<entt::tag<"ui"_hs>>(entity))
  {
    return;
  }

  auto& position = registry.get<Position>(entity);
  m_world.spatial_hash.remove(entity, position.spatial_hash_index);
}
}  // namespace dl
