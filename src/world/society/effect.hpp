#pragma once

#include <entt/fwd.hpp>
#include <variant>

namespace dl
{
struct HungerEffect
{
  int value{};
};

struct HydrationEffect
{
  int value{};
};

using Effect = std::variant<HungerEffect, HydrationEffect>;

struct EffectApplier
{
  EffectApplier(entt::registry& registry, entt::entity entity);

  void operator()(const HungerEffect& effect);
  void operator()(const HydrationEffect& effect);

 private:
  entt::registry& m_registry;
  entt::entity m_entity;
};
}  // namespace dl
