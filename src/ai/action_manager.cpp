#include "./action_manager.hpp"

#include <spdlog/spdlog.h>

#include "ai/actions/utils.hpp"
#include "ai/actions/walk.hpp"
#include "core/game_context.hpp"
#include "core/maths/vector.hpp"
#include "world/world.hpp"

namespace dl::ai
{
ActionManager::ActionManager(GameContext& game_context, World& world)
    : m_game_context(game_context), m_registry(*game_context.registry), m_world(world)
{
}

bool ActionManager::create_tile_job(const JobType job_type, entt::entity entity, const Vector3i& position)
{
  switch (job_type)
  {
  case JobType::Walk:
  {
    action::walk::create_job({.registry = m_registry, .agent_entity = entity, .position = position});
    return true;
  }
  default:
    return utils::create_tile_job({.world = m_world,
                                   .registry = m_registry,
                                   .position = position,
                                   .job_type = job_type,
                                   .selected_entity = entity});
  }
}

bool ActionManager::create_tile_job_bulk(const JobType job_type,
                                         const std::vector<entt::entity>& entities,
                                         const Vector3i& position)
{
  switch (job_type)
  {
  case JobType::Walk:
  {
    for (const auto entity : entities)
    {
      action::walk::create_job({.registry = m_registry, .agent_entity = entity, .position = position});
    }
    return true;
  }
  default:
    return utils::create_tile_job({.world = m_world,
                                   .registry = m_registry,
                                   .position = position,
                                   .job_type = job_type,
                                   .selected_entities = &entities});
  }
}

bool ActionManager::create_item_job(const JobType job_type,
                                    entt::entity entity,
                                    entt::entity item,
                                    const Vector3i& position)
{
  const auto job = m_registry.create();
  m_registry.emplace<Target>(job, position, static_cast<uint32_t>(item));
  m_registry.emplace<JobData>(job, job_type);

  action::walk::create_job({.registry = m_registry, .agent_entity = entity, .position = position});

  auto& agent = m_registry.get<SocietyAgent>(entity);
  agent.jobs.push(Job{2, job});

  return true;
}

bool ActionManager::create_item_job_bulk(const JobType job_type,
                                         const std::vector<entt::entity>& entities,
                                         entt::entity item,
                                         const Vector3i& position)
{
  const auto job = m_registry.create();
  m_registry.emplace<Target>(job, position, static_cast<uint32_t>(item));
  m_registry.emplace<JobData>(job, job_type);

  for (const auto entity : entities)
  {
    // TODO: Abstract tile job creation into a function
    action::walk::create_job({.registry = m_registry, .agent_entity = entity, .position = position});

    auto& agent = m_registry.get<SocietyAgent>(entity);
    agent.jobs.push(Job{2, job});
  }

  return true;
}
}  // namespace dl::ai
