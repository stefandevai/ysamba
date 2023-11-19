#include "./game.hpp"

#include <spdlog/spdlog.h>
#include "../../world/world.hpp"
#include "../components/position.hpp"

namespace dl
{
  GameSystem::GameSystem(entt::registry& registry, World& world)
    : m_world(world)
  {
    registry.on_construct<Position>().connect<&GameSystem::m_add_to_spatial_hash>(this);
  }


  void GameSystem::update()
  { }

  void GameSystem::m_add_to_spatial_hash (entt::registry& registry, entt::entity entity)
  {
    auto& position = registry.get<Position> (entity);
    m_world.spatial_hash.add(entity, position.x * 16, position.y * 16);
  }
}

