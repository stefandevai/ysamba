#include "./operation_manager.hpp"

#include <spdlog/spdlog.h>
#include <entt/core/hashed_string.hpp>
#include <entt/core/type_traits.hpp>

#include "core/random.hpp"
#include "core/game_context.hpp"
#include "core/maths/vector.hpp"
#include "ecs/components/storage_area.hpp"
#include "ecs/components/carried_items.hpp"
#include "ecs/components/job_data.hpp"
#include "ecs/components/item.hpp"
#include "ecs/components/job_data.hpp"
#include "ecs/components/job_progress.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/society_agent.hpp"
#include "ecs/components/weared_items.hpp"
#include "ecs/components/wielded_items.hpp"
#include "world/society/job.hpp"
#include "world/target.hpp"
#include "world/world.hpp"

namespace dl::ai
{
OperationManager::OperationManager(GameContext& game_context, World& world)
    : m_game_context(game_context), m_registry(*game_context.registry), m_world(world)
{
}

std::vector<Operation> OperationManager::get_viable(entt::entity entity)
{
  (void)entity;

  std::vector<Operation> operations{};

  // TODO: Select viable operations based on the entity's and world's current state Use the concept of operation
  // buckets to group similar operations together according to their priority
  operations.push_back(Operation{OperationType::None});
  operations.push_back(Operation{OperationType::Harvest});
  operations.push_back(Operation{OperationType::Store});

  return operations;
}

double OperationManager::compute_score(entt::entity entity, const Operation& operation)
{
  using namespace entt::literals;

  switch (operation.type)
  {
  case OperationType::None:
    return 0.0001;
  case OperationType::Harvest: {
      const auto& position = m_registry.get<Position>(entity);
      const auto& target = m_world.search_by_flag("HARVESTABLE", Vector3i{position.x, position.y, position.z});
      const auto distance_squared = std::pow(target.position.x - position.x, 2) + std::pow(target.position.y - position.y, 2) + std::pow(target.position.z - position.z, 2);

      double score = 0.0;

      if (distance_squared < 1200 /* 20^2*3 */)
      {
        score = 1.0 - distance_squared / 1200.0;
      }

      score = std::clamp(score, 0.0, 1.0);
      return score;
    }
  case OperationType::Store: {
      auto storage_area_view = m_registry.view<StorageArea>();

      if (storage_area_view.size() == 0)
      {
        return 0.0;
      }

      auto storable_view = m_registry.view<entt::tag<"storable"_hs>>();
      double score = storable_view.size() / 20.0;
      score = std::clamp(score, 0.0, 0.99);
      return score;
    }
  default:
    spdlog::critical("Cannot score unknown operation");
    break;
  }

  return 0.0;
}

const Operation& OperationManager::select_best(entt::entity entity, std::vector<Operation>& operations)
{
  assert(!operations.empty());

  std::sort(
      operations.begin(), operations.end(), [](const Operation& a, const Operation& b) { return a.score > b.score; });

  return operations[0];
}

void OperationManager::dispatch(entt::entity entity, const Operation& operation)
{
  switch (operation.type)
  {
  case OperationType::None:
    break;
  case OperationType::Harvest:
    dispatch_harvest(entity);
    break;
  case OperationType::Store:
    dispatch_store(entity);
    break;
  default:
    spdlog::critical("Cannot dispatch unknown operation");
    break;
  }
}

void OperationManager::dispatch_harvest(entt::entity entity)
{
  // Find nearest harvest target
  const auto& position = m_registry.get<Position>(entity);
  const auto& target = m_world.search_by_flag("HARVESTABLE", Vector3i{position.x, position.y, position.z});

  m_create_job(JobType::Harvest, entity, target.position);
}

void OperationManager::dispatch_store(entt::entity entity)
{
  using namespace entt::literals;

  // Find nearest item to store

  // TODO: Implement search in spatial hash
  // const auto target = m_world.spatial_hash.find_with_component<Storable>(Vector3i{position.x, position.y, position.z});

  auto storage_area_view = m_registry.view<StorageArea>();

  // Dirty hack to get a random storage area
  auto r = random::get_integer(0, storage_area_view.size());
  int idx = 0;
  entt::entity storage_area_entity = entt::null;

  for (const auto storage_area : storage_area_view)
  {
    if (idx == r)
    {
      storage_area_entity = storage_area;
    }
    ++idx;
  }

  if (!m_registry.valid(storage_area_entity))
  {
    return;
  }

  auto storable_view = m_registry.view<Position, Item, entt::tag<"storable"_hs>>();

  entt::entity target_entity = entt::null;

  // TODO: Search better way to find an entity within a view
  for (const auto storable : storable_view)
  {
    target_entity = storable;
    break;
  }

  if (!m_registry.valid(target_entity))
  {
    return;
  }


  // Add pickup job
  m_registry.remove<entt::tag<"storable"_hs>>(target_entity);

  const auto& position = m_registry.get<Position>(target_entity);
  const auto job = m_registry.create();
  m_registry.emplace<Target>(job, Vector3i{position.x, position.y, position.z}, static_cast<uint32_t>(target_entity));
  m_registry.emplace<JobData>(job, JobType::Pickup);
  m_assign_job(job, Vector3i{position.x, position.y, position.z}, entity);

  // Add drop job
  const auto storage_position = m_registry.get<Position>(storage_area_entity);
  const auto drop_job = m_registry.create();
  m_registry.emplace<Target>(drop_job, Vector3i{storage_position.x, storage_position.y, storage_position.z}, static_cast<uint32_t>(target_entity));
  m_registry.emplace<JobData>(drop_job, JobType::Drop);
  m_assign_job(drop_job, Vector3i{storage_position.x, storage_position.y, storage_position.z}, entity);
}

void OperationManager::m_create_job(const JobType job_type, entt::entity entity, const Vector3i& position)
{
  const auto& tile = m_world.get(position.x, position.y, position.z);

  if (!tile.actions.contains(job_type))
  {
    return;
  }

  const auto& qualities_required = tile.actions.at(job_type).qualities_required;
  const auto& consumed_items = tile.actions.at(job_type).consumes;

  if (!consumed_items.empty())
  {
    if (!m_has_consumables(consumed_items))
    {
      // TODO: Notify player that items are needed
      return;
    }
  }

  const auto job = m_registry.create();
  m_registry.emplace<Target>(job, position, tile.id);
  m_registry.emplace<JobData>(job, job_type);
  bool job_added = false;

  // Check if the agent has the necessary qualities to perform the action
  if (!qualities_required.empty())
  {
    if (!m_has_qualities_required(qualities_required, entity))
    {
      // TODO: Notify player that items with required qualities are needed
      return;
    }
  }

  m_assign_job(job, position, entity);
  job_added = true;

  if (!job_added)
  {
    m_registry.destroy(job);
  }
}

bool OperationManager::m_has_consumables(const std::map<uint32_t, uint32_t>& consumables)
{
  // TODO: Check society inventory for consumable items
  (void)consumables;
  return true;
}

bool OperationManager::m_has_qualities_required(const std::vector<std::string>& qualities_required, entt::entity entity)
{
  const auto& carried_items = m_registry.get<CarriedItems>(entity);
  const auto& weared_items = m_registry.get<WearedItems>(entity);
  const auto& wielded_items = m_registry.get<WieldedItems>(entity);

  for (const auto& quality : qualities_required)
  {
    bool has_quality = false;

    for (const auto item_entity : carried_items.items)
    {
      const auto& item = m_registry.get<Item>(item_entity);
      const auto& item_data = m_world.get_item_data(item.id);

      if (item_data.qualities.contains(quality))
      {
        has_quality = true;
        continue;
      }
    }

    if (!has_quality)
    {
      for (const auto item_entity : weared_items.items)
      {
        const auto& item = m_registry.get<Item>(item_entity);
        const auto& item_data = m_world.get_item_data(item.id);

        if (item_data.qualities.contains(quality))
        {
          has_quality = true;
          continue;
        }
      }
    }

    if (!has_quality)
    {
      if (m_registry.valid(wielded_items.left_hand))
      {
        const auto& item = m_registry.get<Item>(wielded_items.left_hand);
        const auto& item_data = m_world.get_item_data(item.id);

        if (item_data.qualities.contains(quality))
        {
          has_quality = true;
        }
      }

      if (!has_quality && m_registry.valid(wielded_items.right_hand))
      {
        const auto& item = m_registry.get<Item>(wielded_items.right_hand);
        const auto& item_data = m_world.get_item_data(item.id);

        if (item_data.qualities.contains(quality))
        {
          has_quality = true;
        }
      }
    }

    if (!has_quality)
    {
      return false;
    }
  }
  return true;
}

void OperationManager::m_assign_job(const entt::entity job, const Vector3i& position, entt::entity entity)
{
  const auto walk_job = m_registry.create();
  m_registry.emplace<Target>(walk_job, position);
  m_registry.emplace<JobData>(walk_job, JobType::Walk);

  auto& agent = m_registry.get<SocietyAgent>(entity);
  agent.jobs.push(Job{2, walk_job});
  agent.jobs.push(Job{2, job});
}

}  // namespace dl::ai
