#include <spdlog/spdlog.h>

#include "./ai.hpp"
#include "core/game_context.hpp"
#include "ecs/components/society_agent.hpp"
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
  switch (operation.type)
  {
  case OperationType::None:
    return 0.1;
  case OperationType::Harvest:
    return 1.0;
  case OperationType::Store:
    return 0.5;
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
    break;
  case OperationType::Store:
    break;
  default:
    spdlog::critical("Cannot dispatch unknown operation");
    break;
  }
}
}  // namespace dl::ai
