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
class System
{
 public:
  System(GameContext& game_context, World& world);

  void update(entt::registry& registry);

 private:
  GameContext& m_game_context;
  World& m_world;
  OperationManager m_operation_manager{m_game_context, m_world};
};
}  // namespace dl::ai
