#pragma once

#include <entt/entity/fwd.hpp>

#include "./item_data.hpp"

namespace dl::item_factory
{
entt::entity create(const ItemData& item_data, entt::registry& registry);
}
