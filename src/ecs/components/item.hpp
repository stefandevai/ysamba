#pragma once

namespace dl
{
struct Item
{
  uint32_t id = 0;
};

template <typename Archive>
void serialize(Archive& archive, Item& item)
{
  archive(item.id);
}
}  // namespace dl
