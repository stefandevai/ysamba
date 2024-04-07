#pragma once

#include <entt/entity/fwd.hpp>

#include "./item_data.hpp"

namespace dl
{
class World;
}

namespace dl::item_factory
{
entt::entity create(const ItemData& item_data, entt::registry& registry);
double parse_weight(const std::string& raw_weight);
double parse_volume(const std::string& raw_volume);
std::string weight_to_string(const double weight);
std::string volume_to_string(const double volume);

// Gets a formatted item label for inventory
std::string get_item_label(World& world, entt::registry& registry, entt::entity entity);
}  // namespace dl::item_factory
