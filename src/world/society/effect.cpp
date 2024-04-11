#include "./effect.hpp"

#include <entt/entity/registry.hpp>

#include "ecs/components/biology.hpp"

namespace dl
{
EffectApplier::EffectApplier(entt::registry& registry, entt::entity entity) : m_registry(registry), m_entity(entity) {}

void EffectApplier::operator()(const HungerEffect& effect)
{
  auto& biology = m_registry.get<Biology>(m_entity);

  biology.hunger += effect.value;
}

void EffectApplier::operator()(const HydrationEffect& effect)
{
  auto& biology = m_registry.get<Biology>(m_entity);
  biology.hydration += effect.value;
}
}  // namespace dl
