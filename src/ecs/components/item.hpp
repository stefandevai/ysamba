#pragma once

namespace dl
{
struct Item
{
  uint32_t id = 0;
  entt::entity container = entt::null;
};

template <typename Archive>
void serialize(Archive& archive, Item& item)
{
  archive(item.id, item.container);
}
}  // namespace dl
