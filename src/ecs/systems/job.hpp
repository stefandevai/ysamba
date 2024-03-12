#pragma once

#include <entt/entity/registry.hpp>

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

  void m_update_tile_job(const entt::entity job, const entt::entity agent, entt::registry& registry);
  void m_create_or_assign_job_progress(const entt::entity job, entt::registry& registry);
  void m_create_or_assign_build_hub_job_progress(const entt::entity job, entt::registry& registry);
};
}  // namespace dl
