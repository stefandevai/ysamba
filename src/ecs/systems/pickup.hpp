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
  static entt::entity get_container(entt::registry& registry, entt::entity entity, const ItemData& item_data);
  static bool can_pickup(entt::registry& registry, entt::entity entity, const ItemData& item_data);
  static entt::entity iterate_containers(entt::registry& registry,
                                         const ItemData& item_data,
                                         const std::vector<entt::entity>& items);
  static entt::entity search_container(entt::registry& registry,
                                       const ItemData& item_data,
                                       const std::vector<entt::entity>& items);
  static entt::entity get_liquid_container(entt::registry& registry, entt::entity entity, const ItemData& item_data);

 private:
  World& m_world;

  void m_pickup(entt::registry& registry, entt::entity entity);
  void m_pickup_liquid(entt::registry& registry, entt::entity entity);
};
}  // namespace dl
