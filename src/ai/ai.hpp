#pragma once

#include <entt/entity/registry.hpp>
#include <vector>

namespace dl
{
class World;
}

namespace dl::ai
{
enum class OperationType
{
  None,
  Harvest,
  Store,
};

struct Operation
{
  OperationType type;
  double score;
};

class System
{
 public:
  System(World& world);

  void update(entt::registry& registry);

 private:
  World& m_world;

  std::vector<Operation> m_get_available_operations(entt::registry& registry, entt::entity entity);
  void m_compute_scores(entt::registry& registry, entt::entity entity, std::vector<Operation>& operations);
  Operation m_select_best_operation(entt::registry& registry,
                                    entt::entity entity,
                                    const std::vector<Operation>& operations);
  void m_dispatch_operation(entt::registry& registry, entt::entity entity, const Operation& operation);
};
}  // namespace dl::ai
