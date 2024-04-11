#pragma once

#include <entt/entity/fwd.hpp>

namespace dl
{
class World;
}

namespace dl::utils
{
std::vector<entt::entity> get_selected_entities(entt::registry& registry);
std::vector<entt::entity> get_entity_items(entt::registry& registry, entt::entity entity);
bool has_item(entt::registry& registry, entt::entity entity, entt::entity item);
bool remove_item_from_entity(entt::registry& registry, entt::entity entity, entt::entity item);
void decrease_container_weight_and_volume_by_item(
    World& world, entt::registry& registry, entt::entity entity, entt::entity item, int quantity = -1);
}  // namespace dl::utils
