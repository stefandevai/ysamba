#include "./physics.hpp"

#include <spdlog/spdlog.h>
#include "../../world/world.hpp"
#include "../components/position.hpp"
#include "../components/velocity.hpp"

namespace dl
{
  PhysicsSystem::PhysicsSystem(World& world)
    : m_world(world)
  {
    /* entt::entity entity1{1}; */
    /* entt::entity entity2{2}; */
    /* entt::entity entity3{3}; */
    /* entt::entity entity4{4}; */
    /* entt::entity entity5{5}; */
    /* entt::entity entity6{6}; */
    /* entt::entity entity7{7}; */
    /* entt::entity entity8{8}; */
    /* entt::entity entity9{9}; */
    /* entt::entity entity10{10}; */

    /* m_spatial_hash.add(entity1, 0, 0); */
    /* m_spatial_hash.add(entity2, 1, 1); */
    /* m_spatial_hash.add(entity3, 3, 1); */
    /* m_spatial_hash.add(entity4, 20, 20); */
    /* m_spatial_hash.add(entity5, 21, 20); */
    /* m_spatial_hash.add(entity6, 20, 21); */
    /* m_spatial_hash.add(entity7, 21, 21); */
    /* m_spatial_hash.add(entity8, 50, 50); */
    /* m_spatial_hash.add(entity9, 51, 51); */
    /* m_spatial_hash.add(entity10, 52, 50); */

    /* const auto entities = m_spatial_hash.get(51, 51); */

    /* spdlog::warn("HEREEEEEEEEEEEEEEEEE"); */
    /* for (const auto& entity : entities) */
    /* { */
    /*   spdlog::warn("Entity: {}", static_cast<uint32_t>(entity)); */
    /* } */
  }

  void PhysicsSystem::update(entt::registry &registry, const uint32_t delta)
  {
    auto view = registry.view<Position, Velocity>();
    view.each([this, delta](auto &position, auto &velocity) {
      if (velocity.x == 0 && velocity.y == 0)
      {
        return;
      }

      const auto delta_s = delta / 1000.0;
      const double x_candidate = position.x + velocity.x * delta_s;
      const double y_candidate = position.y + velocity.y * delta_s;
      const double z_candidate = position.z + velocity.z * delta_s;

      const auto& target_tile = m_world.get(x_candidate, y_candidate, z_candidate);

      if (target_tile.flags.contains("WALKABLE"))
      {
        position.x = x_candidate;
        position.y = y_candidate;
      }

      velocity.x = 0.;
      velocity.y = 0.;
      velocity.z = 0.;
    });
  }
}
