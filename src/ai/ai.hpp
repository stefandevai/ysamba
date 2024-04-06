#pragma once

#include <entt/entity/registry.hpp>

#include "ai/operation_manager.hpp"

namespace dl
{
struct GameContext;
class World;
}  // namespace dl

namespace dl::ai
{
class ActionManager;

class System
{
 public:
  System(GameContext& game_context, World& world, ai::ActionManager& action_manager);

  void update(entt::registry& registry);

 private:
  GameContext& m_game_context;
  World& m_world;
  ai::ActionManager& m_action_manager;
  OperationManager m_operation_manager{m_game_context, m_world, m_action_manager};
};
}  // namespace dl::ai
