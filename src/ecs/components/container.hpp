#pragma once

#include <cstdint>
#include <entt/entity/entity.hpp>
#include <vector>

struct Container
{
  double weight_capacity = 0.0;
  double volume_capacity = 0.0;
  double weight_occupied = 0.0;
  double volume_occupied = 0.0;
  std::vector<uint32_t> materials;
  std::vector<entt::entity> items{};
};
