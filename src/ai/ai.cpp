#include "./ai.hpp"

#include <spdlog/spdlog.h>

#include "ai/action_manager.hpp"
#include "core/game_context.hpp"
#include "ecs/components/society_agent.hpp"
#include "world/world.hpp"

namespace dl::ai
{
System::System(GameContext& game_context, World& world, ActionManager& action_manager)
    : m_game_context(game_context), m_world(world), m_action_manager(action_manager)
{
}

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

    // Get operations that we are currently viable to compute score
    auto operations = m_operation_manager.get_viable(entity);

    if (operations.empty())
    {
      continue;
    }

    // Compute score for all these operations
    for (auto& operation : operations)
    {
      operation.score = m_operation_manager.compute_score(entity, operation);
    }

    // Select the best score
    const auto& operation = m_operation_manager.select_best(entity, operations);

    if (operation.type == OperationType::None)
    {
      continue;
    }

    // Convert operation to a series of jobs that can be concretely executed by the agents
    m_operation_manager.dispatch(entity, operation);
  }
}
}  // namespace dl::ai
