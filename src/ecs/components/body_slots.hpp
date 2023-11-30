#pragma once

#include <entt/entity/fwd.hpp>

namespace dl
{
struct BodySlots
{
  entt::entity left_hand = entt::null;
  entt::entity right_hand = entt::null;
  entt::entity head = entt::null;
  entt::entity torso = entt::null;
  entt::entity = entt::null;
};

}  // namespace dl
