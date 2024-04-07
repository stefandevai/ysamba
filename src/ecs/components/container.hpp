#pragma once

#include <cstdint>
#include <entt/entity/entity.hpp>
#include <vector>

#include "world/item_data.hpp"

namespace dl
{
struct Container
{
  double weight_capacity = 0.0;
  double volume_capacity = 0.0;
  double weight_occupied = 0.0;
  double volume_occupied = 0.0;
  std::vector<MatterState> matter_states{};
  std::vector<entt::entity> items{};
};
}  // namespace dl
