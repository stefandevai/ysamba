#include "./ai.hpp"

#include <spdlog/spdlog.h>

#include "ecs/components/society_agent.hpp"
#include "world/world.hpp"

namespace dl::ai
{
System::System(World& world) : m_world(world) {}

void System::update(entt::registry& registry)
{
  const auto society_view = registry.view<SocietyAgent>();

  for (const auto entity : society_view)
  {
    const auto& agent = registry.get<SocietyAgent>(entity);

    // Skip if agent already has any job
    if (!agent.jobs.empty())
    {
      continue;
    }

    // Get operations that we are currently available to compute score
    auto operations = m_get_available_operations(registry, entity);

    if (operations.empty())
    {
      continue;
    }

    // Compute score for all these operations
    m_compute_scores(registry, entity, operations);

    // Select the best score
    auto operation = m_select_best_operation(registry, entity, operations);

    if (operation.type == OperationType::None)
    {
      continue;
    }

    // Convert operation to a series of jobs that can be concretely executed by the agents
    m_dispatch_operation(registry, entity, operation);
  }
}

std::vector<Operation> System::m_get_available_operations(entt::registry& registry, entt::entity entity)
{
  std::vector<Operation> operations{};

  return operations;
}

void System::m_compute_scores(entt::registry& registry, entt::entity entity, std::vector<Operation>& operations)
{
  assert(!operations.empty());
}

Operation System::m_select_best_operation(entt::registry& registry,
                                          entt::entity entity,
                                          const std::vector<Operation>& operations)
{
  assert(!operations.empty());

  return operations[0];
}

void System::m_dispatch_operation(entt::registry& registry, entt::entity entity, const Operation& operation) {}
}  // namespace dl::ai
