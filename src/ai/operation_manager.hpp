#pragma once

#include <entt/entity/registry.hpp>
#include <map>
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

 private:
  GameContext& m_game_context;
  entt::registry& m_registry;
  World& m_world;

  void m_create_job(const JobType job_type, entt::entity entity, const Vector3i& position);
  void m_create_job(const Vector3i& position, const JobType job_type);
  bool m_has_consumables(const std::map<uint32_t, uint32_t>& consumables);
  bool m_has_qualities_required(const std::vector<std::string>& qualities_required, entt::entity entity);
  void m_assign_job(const entt::entity job, const Vector3i& position, entt::entity entity);
};
}  // namespace dl::ai
