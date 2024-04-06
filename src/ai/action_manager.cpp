#include "./action_manager.hpp"

#include <spdlog/spdlog.h>

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
  return m_create_tile_job(
      {.world = m_world, .registry = m_registry, .position = position, .job_type = job_type, .entities = {entity}});
}

bool ActionManager::create_tile_job_bulk(const JobType job_type,
                                         const std::vector<entt::entity>& entities,
                                         const Vector3i& position)
{
  return m_create_tile_job(
      {.world = m_world, .registry = m_registry, .position = position, .job_type = job_type, .entities = entities});
}

bool ActionManager::create_item_job(const JobType job_type,
                                    entt::entity entity,
                                    entt::entity item,
                                    const Vector3i& position)
{
  return m_create_item_job(
      {.registry = m_registry, .position = position, .job_type = job_type, .entities = {entity}, .item = item});
}

bool ActionManager::create_item_job_bulk(const JobType job_type,
                                         const std::vector<entt::entity>& entities,
                                         entt::entity item,
                                         const Vector3i& position)
{
  return m_create_item_job(
      {.registry = m_registry, .position = position, .job_type = job_type, .entities = entities, .item = item});
}

bool ActionManager::m_create_tile_job(utils::CreateTileJobParams params)
{
  switch (params.job_type)
  {
  case JobType::Harvest:
  case JobType::Break:
  case JobType::Dig:
  case JobType::PrepareFirecamp:
  case JobType::StartFire:
  case JobType::PlaceHutExterior:
  case JobType::ConstructEntrance:
    return utils::create_tile_job(std::move(params));
  default:
  {
    spdlog::critical("Tile job not implemented");
    return false;
  }
  }
}

bool ActionManager::m_create_item_job(utils::CreateItemJobParams params)
{
  switch (params.job_type)
  {
  case JobType::Pickup:
  case JobType::Wear:
  case JobType::Wield:
  case JobType::Drop:
    return utils::create_item_job(std::move(params));
  default:
  {
    spdlog::critical("Item job not implemented");
    return false;
  }
  }
}

}  // namespace dl::ai
