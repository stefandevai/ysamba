#pragma once

#include <entt/entity/registry.hpp>
#include <vector>

#include "ai/operation.hpp"
#include "world/society/job_type.hpp"

namespace dl
{
struct GameContext;
class World;
struct Vector3i;
}  // namespace dl

namespace dl::ai
{
class OperationManager
{
 public:
  OperationManager(GameContext& game_context, World& world);

  std::vector<Operation> get_viable(entt::entity entity);
  double compute_score(entt::entity entity, const Operation& operation);
  const Operation& select_best(entt::entity entity, std::vector<Operation>& operations);
  void dispatch(entt::entity entity, const Operation& operation);

  // Disptach functions
  void dispatch_harvest(entt::entity entity);
  void dispatch_store(entt::entity entity);
  void dispatch_eat(entt::entity entity);

 private:
  GameContext& m_game_context;
  entt::registry& m_registry;
  World& m_world;
};
}  // namespace dl::ai
