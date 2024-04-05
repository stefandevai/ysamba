#pragma once

#include <entt/entity/registry.hpp>
#include <vector>

#include "world/society/job_type.hpp"

namespace dl
{
struct GameContext;
class World;
struct Vector3i;
}  // namespace dl

namespace dl::ai
{
class ActionManager
{
 public:
  ActionManager(GameContext& game_context, World& world);

  bool create_tile_job(const JobType job_type, entt::entity entity, const Vector3i& position);
  bool create_tile_job_bulk(const JobType job_type,
                            const std::vector<entt::entity>& entities,
                            const Vector3i& position);
  bool create_item_job(const JobType job_type, entt::entity entity, entt::entity item, const Vector3i& position);
  bool create_item_job_bulk(const JobType job_type,
                            const std::vector<entt::entity>& entities,
                            entt::entity item,
                            const Vector3i& position);

 private:
  GameContext& m_game_context;
  entt::registry& m_registry;
  World& m_world;
};
}  // namespace dl::ai
