#pragma once

#include <entt/entity/fwd.hpp>

#include "./item_data.hpp"

namespace dl::item_factory
{
entt::entity create(const ItemData& item_data, entt::registry& registry);
double parse_weight(const std::string& raw_weight);
double parse_volume(const std::string& raw_volume);
std::string weight_to_string(const double weight);
std::string volume_to_string(const double volume);
}  // namespace dl::item_factory
