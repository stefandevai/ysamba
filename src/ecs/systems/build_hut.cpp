#include "./build_hut.hpp"

#include <spdlog/spdlog.h>

#include "core/random.hpp"
#include "ecs/components/action_build_hut.hpp"
#include "ecs/components/action_place_hut_exterior.hpp"
#include "ecs/components/action_walk.hpp"
#include "ecs/components/biology.hpp"
#include "ecs/components/home.hpp"
#include "ecs/components/home_floor.hpp"
#include "ecs/components/job_data.hpp"
#include "ecs/components/job_data_build_hut.hpp"
#include "ecs/components/job_progress.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/society_agent.hpp"
#include "world/target.hpp"
#include "world/world.hpp"

namespace dl
{
const auto stop_build_hut = [](entt::registry& registry, const entt::entity entity, const entt::entity job) {
  auto& job_data = registry.get<JobData>(job);
  auto& progress = registry.get<JobProgress>(job_data.progress_entity);
  progress.agents.erase(std::remove(progress.agents.begin(), progress.agents.end(), entity));
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

    if (job_data.status != JobStatus::InProgress)
    {
      continue;
    }

    assert(registry.all_of<JobProgress>(job_data.progress_entity));
    auto& job_progress = registry.get<JobProgress>(job_data.progress_entity);

    if (job_progress.progress < job_progress.total_cost)
    {
      // Consumate work
      auto& biology = registry.get<Biology>(entity);

      if (biology.energy <= biology.work_cost)
      {
        continue;
      }

      biology.energy -= biology.work_cost;
      job_progress.progress += 100;
      m_random_walk(registry, entity, action_build_hut.job);
      continue;
    }

    // Job is done
    // Walk outside of hut bounds
    // Agent is already walking
    if (registry.all_of<ActionWalk>(entity))
    {
      continue;
    }

    // NOTE: Taking target as a reference causes a weird bug where the top part
    // is placed placed with an offset such as target is modified in some way.
    // This is a workaround.
    const auto& target = registry.get<Position>(job_data.progress_entity);
    auto& job_data_build_hut = registry.get<JobDataBuildHut>(action_build_hut.job);
    const uint32_t hut_size = job_data_build_hut.hut_size;
    const auto& position = registry.get<Position>(entity);

    if (m_is_within_hut_bounds(position, target, hut_size))
    {
      const uint32_t distance_to_bottom = (target.y + hut_size) - position.y;
      const uint32_t distance_to_top = position.y - (target.y - 1);
      const uint32_t distance_to_left = position.x - (target.x - 1);
      const uint32_t distance_to_right = (target.x + hut_size) - position.x;

      const uint32_t min = std::min({distance_to_bottom, distance_to_top, distance_to_left, distance_to_right});

      Vector3i new_position{position.x, position.y, position.z};

      if (min == distance_to_bottom)
      {
        new_position.y = target.y + hut_size;
      }
      else if (min == distance_to_top)
      {
        new_position.y = target.y - 1;
      }
      else if (min == distance_to_left)
      {
        new_position.x = target.x - 1;
      }
      else if (min == distance_to_right)
      {
        new_position.x = target.x + hut_size;
      }

      const auto walk_job = registry.create();
      registry.emplace<Target>(walk_job, new_position, 0, 0);
      registry.emplace<JobData>(walk_job, JobType::Walk);

      auto& agent = registry.get<SocietyAgent>(entity);
      agent.jobs.push(Job{0, walk_job});

      continue;
    }

    // Agent is not within hut bounds.
    // If there are still agents within the hut bounds, remove the build hut action, freeing them for other jobs
    if (job_progress.agents.size() > 1)
    {
      stop_build_hut(registry, entity, action_build_hut.job);
      continue;
    }

    // There are no more agents within the hut bounds. Place the hut and remove the build hut action
    // First, remove preview placeholder
    for (const auto entity : registry.view<entt::tag<"hut_preview"_hs>>())
    {
      registry.destroy(entity);
    }

    assert(hut_size >= 3);

    const auto home_entity = registry.create();
    registry.emplace<Position>(home_entity, target.x, target.y, target.z);
    registry.emplace<Home>(home_entity, 0, Vector3i{static_cast<int>(hut_size), static_cast<int>(hut_size), 2});

    // Place hut structure tiles
    if (hut_size == 3)
    {
      // Create entities for the home floor
      std::array<entt::entity, 8> home_floor{};

      for (uint32_t i = 0; i < home_floor.size(); ++i)
      {
        home_floor[i] = registry.create();
      }

      // Top
      // NOTE: If this is set after the perimeter, for some reason the const reference to target is modified.
      // Weird bug. Maybe entt causes it? An issue with references?
      m_world.set_decoration(148, target.x + 1, target.y + 1, target.z + 1);

      // Perimeter
      Vector3 home_floor_position = Vector3{target.x, target.y, target.z};
      m_world.set_decoration(139, target.x, target.y, target.z);
      registry.emplace<Position>(home_floor[0], home_floor_position);
      registry.emplace<HomeFloor>(home_floor[0], home_entity);

      home_floor_position = Vector3{target.x + 1, target.y, target.z};
      m_world.set_decoration(140, target.x + 1, target.y, target.z);
      registry.emplace<Position>(home_floor[1], home_floor_position);
      registry.emplace<HomeFloor>(home_floor[1], home_entity);

      home_floor_position = Vector3{target.x + 2, target.y, target.z};
      m_world.set_decoration(141, target.x + 2, target.y, target.z);
      registry.emplace<Position>(home_floor[2], home_floor_position);
      registry.emplace<HomeFloor>(home_floor[2], home_entity);

      home_floor_position = Vector3{target.x, target.y + 1, target.z};
      m_world.set_decoration(142, target.x, target.y + 1, target.z);
      registry.emplace<Position>(home_floor[3], home_floor_position);
      registry.emplace<HomeFloor>(home_floor[3], home_entity);

      home_floor_position = Vector3{target.x + 2, target.y + 1, target.z};
      m_world.set_decoration(144, target.x + 2, target.y + 1, target.z);
      registry.emplace<Position>(home_floor[4], home_floor_position);
      registry.emplace<HomeFloor>(home_floor[4], home_entity);

      home_floor_position = Vector3{target.x, target.y + 2, target.z};
      m_world.set_decoration(145, target.x, target.y + 2, target.z);
      registry.emplace<Position>(home_floor[5], home_floor_position);
      registry.emplace<HomeFloor>(home_floor[5], home_entity);

      home_floor_position = Vector3{target.x + 1, target.y + 2, target.z};
      m_world.set_decoration(146, target.x + 1, target.y + 2, target.z);
      registry.emplace<Position>(home_floor[6], home_floor_position);
      registry.emplace<HomeFloor>(home_floor[6], home_entity);

      home_floor_position = Vector3{target.x + 2, target.y + 2, target.z};
      m_world.set_decoration(147, target.x + 2, target.y + 2, target.z);
      registry.emplace<Position>(home_floor[7], home_floor_position);
      registry.emplace<HomeFloor>(home_floor[7], home_entity);
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

    stop_build_hut(registry, entity, action_build_hut.job);
    registry.destroy(job_data.progress_entity);
  }

  auto place_hut_view = registry.view<ActionPlaceHutExterior, Biology, const Position>();
  for (const auto entity : place_hut_view)
  {
    const auto& action = registry.get<ActionPlaceHutExterior>(entity);
    const auto& target = registry.get<Target>(action.job);
    const auto floor_entity = m_world.spatial_hash.get_by_component<HomeFloor>(
        target.position.x, target.position.y, target.position.z, registry);

    if (!registry.valid(floor_entity))
    {
      // TODO: Stop action
      continue;
    }

    const auto& home_floor = registry.get<HomeFloor>(floor_entity);
    const auto& home = registry.get<Home>(home_floor.home_entity);
    const auto& home_position = registry.get<Position>(home_floor.home_entity);

    auto& job_data = registry.get<JobData>(action.job);
    job_data.status = JobStatus::Finished;

    spdlog::debug("Placing hut exterior at ({}, {}, {})", home_position.x, home_position.y, home_position.z);
  }
}

void BuildHutSystem::m_random_walk(entt::registry& registry, const entt::entity entity, const entt::entity job)
{
  // Don't walk if agent is already walking
  if (registry.all_of<ActionWalk>(entity))
  {
    return;
  }

  // Randomly walk during action
  const auto dice_roll = random::get_integer(0, 100);

  if (dice_roll < 20)
  {
    auto& job_data = registry.get<JobData>(job);
    job_data.status = JobStatus::Waiting;
    auto& job_data_build_hut = registry.get<JobDataBuildHut>(job);
    const uint32_t hut_size = job_data_build_hut.hut_size;
    const auto& job_position = registry.get<Position>(job_data.progress_entity);

    const auto offset_x = random::get_integer(0, hut_size);
    const auto offset_y = random::get_integer(0, hut_size);

    const auto new_position = Vector3i{job_position.x + offset_x, job_position.y + offset_y, job_position.z};

    const auto walk_job = registry.create();
    registry.emplace<Target>(walk_job, new_position);
    registry.emplace<JobData>(walk_job, JobType::Walk);

    auto& agent = registry.get<SocietyAgent>(entity);
    agent.jobs.push(Job{0, walk_job});
  }
}

bool BuildHutSystem::m_is_within_hut_bounds(const Position& agent_position,
                                            const Position& hut_position,
                                            const uint32_t hut_size)
{
  return agent_position.x >= hut_position.x && agent_position.x < hut_position.x + hut_size
         && agent_position.y >= hut_position.y && agent_position.y < hut_position.y + hut_size;
}

}  // namespace dl
