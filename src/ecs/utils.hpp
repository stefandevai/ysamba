#pragma once

#include <entt/entity/fwd.hpp>

namespace dl::utils
{
std::vector<entt::entity> get_selected_entities(entt::registry& registry);
std::vector<entt::entity> get_entity_items(entt::registry& registry, entt::entity entity);
bool has_item(entt::registry& registry, entt::entity entity, entt::entity item);
bool remove_item_from_entity(entt::registry& registry, entt::entity entity, entt::entity item);
}  // namespace dl::utils
