#include "./wear.hpp"

#include <spdlog/spdlog.h>

#include "ecs/components/action_wear.hpp"
#include "ecs/components/biology.hpp"
#include "ecs/components/item.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/weared_items.hpp"
#include "world/world.hpp"

namespace dl
{
const auto stop_wear = [](entt::registry& registry, const entt::entity entity, const Job* job) {
  registry.remove<ActionWear>(entity);
  job->status = JobStatus::Finished;
};

WearSystem::WearSystem(World& world) : m_world(world) {}

void WearSystem::update(entt::registry& registry)
{
  auto view = registry.view<ActionWear, Biology, const Position>();
  for (const auto entity : view)
  {
    auto& action_wear = registry.get<ActionWear>(entity);
    const auto& job = action_wear.job;
    const auto& target = job->target;
    const entt::entity item = static_cast<entt::entity>(target.id);

    if (!registry.valid(item))
    {
      stop_wear(registry, entity, job);
      continue;
    }

    // Check if target tile is still there
    if (!m_world.spatial_hash.has(item, target.position.x, target.position.y, target.position.z))
    {
      stop_wear(registry, entity, job);
      continue;
    }

    auto& item_component = registry.get<Item>(item);
    const auto& item_data = m_world.get_item_data(item_component.id);
    const auto& biology = registry.get<Biology>(entity);

    const auto& body_parts = biology.body_parts;
    bool has_all_body_parts = true;

    for (const auto body_part : item_data.weared_on)
    {
      if (std::find(body_parts.begin(), body_parts.end(), body_part) == body_parts.end())
      {
        has_all_body_parts = false;
        break;
      }
    }

    if (has_all_body_parts)
    {
      registry.remove<Position>(item);
      registry.remove<Visibility>(item);

      auto& weared_items = registry.get<WearedItems>(entity);
      weared_items.items.push_back(item);
    }
    else
    {
      // TODO: Handle when agent doesn't have the necessary body parts
      spdlog::warn("Agent doesn't have the body parts to wear '{}'", item_data.name);
    }

    stop_wear(registry, entity, job);
  }
}

}  // namespace dl
