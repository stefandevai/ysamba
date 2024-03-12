#include "./build_hut.hpp"

#include <spdlog/spdlog.h>

#include "ecs/components/action_build_hut.hpp"
#include "ecs/components/job_data.hpp"
#include "ecs/components/job_data_build_hut.hpp"
#include "ecs/components/job_progress.hpp"
#include "ecs/components/position.hpp"
#include "world/target.hpp"
#include "world/world.hpp"

namespace dl
{
const auto stop_build_hut = [](entt::registry& registry, const entt::entity entity, const entt::entity job) {
  auto& job_data = registry.get<JobData>(job);
  job_data.status = JobStatus::Finished;
  registry.remove<ActionBuildHut>(entity);
};

BuildHutSystem::BuildHutSystem(World& world) : m_world(world) {}

void BuildHutSystem::update(entt::registry& registry)
{
  auto view = registry.view<ActionBuildHut, const Position>();
  for (const auto entity : view)
  {
    auto& action_build_hut = registry.get<ActionBuildHut>(entity);
    auto& job_data = registry.get<JobData>(action_build_hut.job);

    if (!registry.valid(job_data.progress_entity))
    {
      stop_build_hut(registry, entity, action_build_hut.job);
      continue;
    }

    auto& job_progress = registry.get<JobProgress>(job_data.progress_entity);
    job_progress.time_left -= 0.001;

    if (job_progress.time_left > 0.0)
    {
      continue;
    }

    registry.destroy(job_data.progress_entity);

    const auto& target = registry.get<Target>(action_build_hut.job);
    auto& job_data_build_hut = registry.get<JobDataBuildHut>(action_build_hut.job);

    const uint32_t hut_size = job_data_build_hut.hut_size;

    assert(hut_size >= 3);

    if (hut_size == 3)
    {
      // Perimeter
      m_world.set_decoration(139, target.position.x, target.position.y, target.position.z);
      m_world.set_decoration(140, target.position.x + 1, target.position.y, target.position.z);
      m_world.set_decoration(141, target.position.x + 2, target.position.y, target.position.z);
      m_world.set_decoration(142, target.position.x, target.position.y + 1, target.position.z);
      m_world.set_decoration(144, target.position.x + 2, target.position.y + 1, target.position.z);
      m_world.set_decoration(145, target.position.x, target.position.y + 2, target.position.z);
      m_world.set_decoration(146, target.position.x + 1, target.position.y + 2, target.position.z);
      m_world.set_decoration(147, target.position.x + 2, target.position.y + 2, target.position.z);

      // Top
      m_world.set_decoration(148, target.position.x + 1, target.position.y + 1, target.position.z + 1);
    }
    else if (hut_size == 4)
    {
      // Perimeter
      m_world.set_decoration(149, target.position.x, target.position.y, target.position.z);
      m_world.set_decoration(155, target.position.x + 3, target.position.y, target.position.z);
      m_world.set_decoration(152, target.position.x, target.position.y + 1, target.position.z);
      m_world.set_decoration(153, target.position.x + 3, target.position.y + 1, target.position.z);
      m_world.set_decoration(175, target.position.x, target.position.y + 2, target.position.z);
      m_world.set_decoration(181, target.position.x + 3, target.position.y + 2, target.position.z);
      m_world.set_decoration(182, target.position.x, target.position.y + 3, target.position.z);
      m_world.set_decoration(183, target.position.x + 1, target.position.y + 3, target.position.z);
      m_world.set_decoration(187, target.position.x + 2, target.position.y + 3, target.position.z);
      m_world.set_decoration(188, target.position.x + 3, target.position.y + 3, target.position.z);

      // Top
      m_world.set_decoration(150, target.position.x + 1, target.position.y + 1, target.position.z + 1);
      m_world.set_decoration(154, target.position.x + 2, target.position.y + 1, target.position.z + 1);
      m_world.set_decoration(169, target.position.x + 1, target.position.y + 2, target.position.z + 1);
      m_world.set_decoration(173, target.position.x + 2, target.position.y + 2, target.position.z + 1);
    }
    else
    {
      // Perimeter structure parts
      m_world.set_decoration(149, target.position.x, target.position.y, target.position.z);
      m_world.set_decoration(155, target.position.x + hut_size - 1, target.position.y, target.position.z);
      m_world.set_decoration(156, target.position.x, target.position.y + 1, target.position.z);
      m_world.set_decoration(159, target.position.x + hut_size - 1, target.position.y + 1, target.position.z);

      m_world.set_decoration(175, target.position.x, target.position.y + hut_size - 2, target.position.z);
      m_world.set_decoration(
          181, target.position.x + hut_size - 1, target.position.y + hut_size - 2, target.position.z);
      m_world.set_decoration(182, target.position.x, target.position.y + hut_size - 1, target.position.z);
      m_world.set_decoration(183, target.position.x + 1, target.position.y + hut_size - 1, target.position.z);
      m_world.set_decoration(
          187, target.position.x + hut_size - 2, target.position.y + hut_size - 1, target.position.z);
      m_world.set_decoration(
          188, target.position.x + hut_size - 1, target.position.y + hut_size - 1, target.position.z);

      // Top structure parts
      m_world.set_decoration(150, target.position.x + 1, target.position.y + 1, target.position.z + 1);
      m_world.set_decoration(154, target.position.x + hut_size - 2, target.position.y + 1, target.position.z + 1);
      m_world.set_decoration(169, target.position.x + 1, target.position.y + hut_size - 2, target.position.z + 1);
      m_world.set_decoration(
          173, target.position.x + hut_size - 2, target.position.y + hut_size - 2, target.position.z + 1);

      // Tiled parts
      for (uint32_t i = 2; i < hut_size - 2; ++i)
      {
        // Horizontal structure parts
        m_world.set_decoration(151, target.position.x + i, target.position.y + 1, target.position.z + 1);
        m_world.set_decoration(170, target.position.x + i, target.position.y + hut_size - 2, target.position.z + 1);
        m_world.set_decoration(184, target.position.x + i, target.position.y + hut_size - 1, target.position.z);

        // Vertical structure parts
        m_world.set_decoration(157, target.position.x + 1, target.position.y + i, target.position.z + 1);
        m_world.set_decoration(158, target.position.x + hut_size - 2, target.position.y + i, target.position.z + 1);
        m_world.set_decoration(160, target.position.x, target.position.y + i, target.position.z);
        m_world.set_decoration(163, target.position.x + hut_size - 1, target.position.y + i, target.position.z);
      }
    }

    stop_build_hut(registry, entity, action_build_hut.job);
  }
}

}  // namespace dl
