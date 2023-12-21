#pragma once

#include <entt/entity/entity.hpp>

namespace dl
{
struct WieldedItems
{
  entt::entity left_hand = entt::null;
  entt::entity right_hand = entt::null;
};

template <typename Archive>
void serialize(Archive& archive, WieldedItems& wielded)
{
  archive(wielded.left_hand, wielded.right_hand);
}
}  // namespace dl
