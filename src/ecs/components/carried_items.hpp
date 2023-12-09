#pragma once

#include <vector>

namespace dl
{
struct CarriedItems
{
  std::vector<entt::entity> items{};
};

template <typename Archive>
void serialize(Archive& archive, CarriedItems& items)
{
  archive(items.items);
}
}  // namespace dl
