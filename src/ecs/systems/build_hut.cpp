#include "./build_hut.hpp"

#include <spdlog/spdlog.h>

#include "ecs/components/action_build_hut.hpp"
#include "ecs/components/biology.hpp"
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
  using namespace entt::literals;

  auto view = registry.view<ActionBuildHut, Biology, const Position>();
  for (const auto entity : view)
  {
    auto& action_build_hut = registry.get<ActionBuildHut>(entity);
    auto& job_data = registry.get<JobData>(action_build_hut.job);

    if (!registry.valid(job_data.progress_entity))
    {
      stop_build_hut(registry, entity, action_build_hut.job);
      continue;
    }

    auto& biology = registry.get<Biology>(entity);

    if (biology.energy <= biology.work_cost)
    {
      continue;
    }

    biology.energy -= biology.work_cost;

    assert(registry.all_of<JobProgress>(job_data.progress_entity));
    auto& job_progress = registry.get<JobProgress>(job_data.progress_entity);
    job_progress.progress += 100;

    if (job_progress.progress < job_progress.total_cost)
    {
      continue;
    }

    // Remove preview placeholder
    for (const auto entity : registry.view<entt::tag<"hut_preview"_hs>>())
    {
      registry.destroy(entity);
    }

    const auto& target = registry.get<Position>(job_data.progress_entity);
    auto& job_data_build_hut = registry.get<JobDataBuildHut>(action_build_hut.job);

    const uint32_t hut_size = job_data_build_hut.hut_size;

    assert(hut_size >= 3);

    if (hut_size == 3)
    {
      // Perimeter
      m_world.set_decoration(139, target.x, target.y, target.z);
      m_world.set_decoration(140, target.x + 1, target.y, target.z);
      m_world.set_decoration(141, target.x + 2, target.y, target.z);
      m_world.set_decoration(142, target.x, target.y + 1, target.z);
      m_world.set_decoration(144, target.x + 2, target.y + 1, target.z);
      m_world.set_decoration(145, target.x, target.y + 2, target.z);
      m_world.set_decoration(146, target.x + 1, target.y + 2, target.z);
      m_world.set_decoration(147, target.x + 2, target.y + 2, target.z);

      // Top
      m_world.set_decoration(148, target.x + 1, target.y + 1, target.z + 1);
    }
    else if (hut_size == 4)
    {
      // Perimeter
      m_world.set_decoration(149, target.x, target.y, target.z);
      m_world.set_decoration(155, target.x + 3, target.y, target.z);
      m_world.set_decoration(152, target.x, target.y + 1, target.z);
      m_world.set_decoration(153, target.x + 3, target.y + 1, target.z);
      m_world.set_decoration(175, target.x, target.y + 2, target.z);
      m_world.set_decoration(181, target.x + 3, target.y + 2, target.z);
      m_world.set_decoration(182, target.x, target.y + 3, target.z);
      m_world.set_decoration(183, target.x + 1, target.y + 3, target.z);
      m_world.set_decoration(187, target.x + 2, target.y + 3, target.z);
      m_world.set_decoration(188, target.x + 3, target.y + 3, target.z);

      // Top
      m_world.set_decoration(150, target.x + 1, target.y + 1, target.z + 1);
      m_world.set_decoration(154, target.x + 2, target.y + 1, target.z + 1);
      m_world.set_decoration(169, target.x + 1, target.y + 2, target.z + 1);
      m_world.set_decoration(173, target.x + 2, target.y + 2, target.z + 1);
    }
    else
    {
      // Perimeter structure parts
      m_world.set_decoration(149, target.x, target.y, target.z);
      m_world.set_decoration(155, target.x + hut_size - 1, target.y, target.z);
      m_world.set_decoration(156, target.x, target.y + 1, target.z);
      m_world.set_decoration(159, target.x + hut_size - 1, target.y + 1, target.z);

      m_world.set_decoration(175, target.x, target.y + hut_size - 2, target.z);
      m_world.set_decoration(181, target.x + hut_size - 1, target.y + hut_size - 2, target.z);
      m_world.set_decoration(182, target.x, target.y + hut_size - 1, target.z);
      m_world.set_decoration(183, target.x + 1, target.y + hut_size - 1, target.z);
      m_world.set_decoration(187, target.x + hut_size - 2, target.y + hut_size - 1, target.z);
      m_world.set_decoration(188, target.x + hut_size - 1, target.y + hut_size - 1, target.z);

      // Top structure parts
      m_world.set_decoration(150, target.x + 1, target.y + 1, target.z + 1);
      m_world.set_decoration(154, target.x + hut_size - 2, target.y + 1, target.z + 1);
      m_world.set_decoration(169, target.x + 1, target.y + hut_size - 2, target.z + 1);
      m_world.set_decoration(173, target.x + hut_size - 2, target.y + hut_size - 2, target.z + 1);

      // Tiled parts
      for (uint32_t i = 2; i < hut_size - 2; ++i)
      {
        // Horizontal structure parts
        m_world.set_decoration(151, target.x + i, target.y + 1, target.z + 1);
        m_world.set_decoration(170, target.x + i, target.y + hut_size - 2, target.z + 1);
        m_world.set_decoration(184, target.x + i, target.y + hut_size - 1, target.z);

        // Vertical structure parts
        m_world.set_decoration(157, target.x + 1, target.y + i, target.z + 1);
        m_world.set_decoration(158, target.x + hut_size - 2, target.y + i, target.z + 1);
        m_world.set_decoration(160, target.x, target.y + i, target.z);
        m_world.set_decoration(163, target.x + hut_size - 1, target.y + i, target.z);
      }
    }

    registry.destroy(job_data.progress_entity);
    stop_build_hut(registry, entity, action_build_hut.job);
  }
}

}  // namespace dl
