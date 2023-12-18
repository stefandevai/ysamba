#pragma once

#include <entt/entity/registry.hpp>

namespace dl
{
class World;
struct ItemData;

class PickupSystem
{
 public:
  PickupSystem(World& world);

  void update(entt::registry& registry);
  static entt::entity get_container_with_enough_capacity(entt::registry& registry,
                                                         entt::entity entity,
                                                         const ItemData& item_data);
  static bool can_pickup(entt::registry& registry, entt::entity entity, const ItemData& item_data);

 private:
  World& m_world;
};
}  // namespace dl
