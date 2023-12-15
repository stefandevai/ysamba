#pragma once

#include <vector>

namespace dl
{
struct WearedItems
{
  std::vector<entt::entity> items{};
};

template <typename Archive>
void serialize(Archive& archive, WearedItems& items)
{
  archive(items.items);
}
}  // namespace dl
