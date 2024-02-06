#pragma once

#include <entt/entity/entity.hpp>

namespace dl
{
struct EnterTurnBasedEvent
{
  entt::entity entity;
};

struct LeaveTurnBasedEvent
{
};

struct UpdateGameEvent
{
};
}  // namespace dl
