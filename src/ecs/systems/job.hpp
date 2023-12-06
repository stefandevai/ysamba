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

  void update(entt::registry& registry);

 private:
  World& m_world;

  template <typename T>
  void m_create_tile_action(const entt::entity entity, entt::registry& registry, const Job& current_job)
  {
    const auto& tile = m_world.get(current_job.target.x, current_job.target.y, current_job.target.z);
    const auto& position = registry.get<Position>(entity);
    const Vector3i tile_position = {std::round(position.x), std::round(position.y), std::round(position.z)};
    auto& action = registry.emplace<T>(
        entity, TileTarget(tile.id, current_job.target.x, current_job.target.y, current_job.target.z));
    action.target.path = m_world.get_path_between(tile_position, current_job.target);
  }

  template <typename T>
  void m_create_item_action(const entt::entity entity, entt::registry& registry, const Job& current_job)
  {
    const auto item =
        m_world.spatial_hash.get_by_component<Pickable>(current_job.target.x, current_job.target.y, registry);
    const auto& position = registry.get<Position>(entity);
    const Vector3i tile_position = {std::round(position.x), std::round(position.y), std::round(position.z)};
    auto& action =
        registry.emplace<T>(entity, ItemTarget(item, current_job.target.x, current_job.target.y, current_job.target.z));
    action.target.path = m_world.get_path_between(tile_position, current_job.target);
  }
};
}  // namespace dl
