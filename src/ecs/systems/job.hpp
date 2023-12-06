#pragma once

#include <entt/entity/registry.hpp>

#include "ecs/components/pickable.hpp"
#include "world/item_target.hpp"
#include "world/society/job.hpp"
#include "world/tile_target.hpp"
#include "world/world.hpp"

namespace dl
{
class JobSystem
{
 public:
  JobSystem(World& world);

  void update(entt::registry& registry, const double delta);

 private:
  World& m_world;

  void m_update_tile_job(const Job& job, const double delta, entt::registry& registry);

  template <typename T>
  void m_create_tile_action(const entt::entity entity, entt::registry& registry, const Job& current_job)
  {
    const auto& tile =
        m_world.get(current_job.target.position.x, current_job.target.position.y, current_job.target.position.z);
    const auto& position = registry.get<Position>(entity);
    const Vector3i tile_position = {std::round(position.x), std::round(position.y), std::round(position.z)};
    auto& action = registry.emplace<T>(
        entity,
        TileTarget(
            tile.id, current_job.target.position.x, current_job.target.position.y, current_job.target.position.z));
    action.target.path = m_world.get_path_between(tile_position, current_job.target.position);
  }

  template <typename T>
  void m_create_item_action(const entt::entity entity, entt::registry& registry, const Job& current_job)
  {
    const auto item = m_world.spatial_hash.get_by_component<Pickable>(
        current_job.target.position.x, current_job.target.position.y, registry);
    const auto& position = registry.get<Position>(entity);
    const Vector3i tile_position = {std::round(position.x), std::round(position.y), std::round(position.z)};
    auto& action = registry.emplace<T>(
        entity,
        ItemTarget(item, current_job.target.position.x, current_job.target.position.y, current_job.target.position.z));
    action.target.path = m_world.get_path_between(tile_position, current_job.target.position);
  }
};
}  // namespace dl
