#pragma once

namespace dl
{
struct ItemStack
{
  uint32_t quantity = 0;
};

template <typename Archive>
void serialize(Archive& archive, ItemStack& stack)
{
  archive(stack.quantity);
}
}  // namespace dl

