#include "./operation_manager.hpp"

#include <spdlog/spdlog.h>

#include <entt/core/hashed_string.hpp>
#include <entt/core/type_traits.hpp>

#include "ai/actions/generic_item.hpp"
#include "ai/actions/generic_tile.hpp"
#include "core/game_context.hpp"
#include "core/maths/vector.hpp"
#include "core/random.hpp"
#include "ecs/components/item.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/storage_area.hpp"
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
  // operations.push_back(Operation{OperationType::Harvest});
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
  case OperationType::Harvest:
  {
    const auto& position = m_registry.get<Position>(entity);
    const auto& target = m_world.search_by_flag("HARVESTABLE", Vector3i{position.x, position.y, position.z});
    const auto distance_squared = std::pow(target.position.x - position.x, 2)
                                  + std::pow(target.position.y - position.y, 2)
                                  + std::pow(target.position.z - position.z, 2);

    double score = 0.0;

    if (distance_squared < 1200 /* 20^2*3 */)
    {
      score = 1.0 - distance_squared / 1200.0;
    }

    score = std::clamp(score, 0.0, 1.0);
    return score;
  }
  case OperationType::Store:
  {
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
  case OperationType::Eat:
    dispatch_eat(entity);
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

  action::generic_tile::job({
      .world = m_world,
      .registry = m_registry,
      .position = target.position,
      .job_type = JobType::Harvest,
      .entities = {entity},
  });
}

void OperationManager::dispatch_store(entt::entity entity)
{
  using namespace entt::literals;

  // Find nearest item to store

  // TODO: Implement search in spatial hash
  // const auto target = m_world.spatial_hash.find_with_component<Storable>(Vector3i{position.x, position.y,
  // position.z});

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
  action::generic_item::job({
      .registry = m_registry,
      .position = Vector3i{position.x, position.y, position.z},
      .job_type = JobType::Pickup,
      .entities = {entity},
      .item = target_entity,
  });

  // Add drop job
  const auto storage_position = m_registry.get<Position>(storage_area_entity);
  action::generic_item::job({
      .registry = m_registry,
      .position = Vector3i{storage_position.x, storage_position.y, storage_position.z},
      .job_type = JobType::Drop,
      .entities = {entity},
      .item = target_entity,
  });
}

void OperationManager::dispatch_eat(entt::entity entity)
{
  using namespace entt::literals;
}

}  // namespace dl::ai
