#include "./wear.hpp"

#include <spdlog/spdlog.h>

#include "ecs/components/action_wear.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/weared_items.hpp"
#include "world/world.hpp"

namespace dl
{
const auto stop_wear = [](entt::registry& registry, const entt::entity entity, const Job* job) {
  spdlog::debug("HEREE7");
  registry.remove<ActionWear>(entity);
  spdlog::debug("HEREE8");
  job->status = JobStatus::Finished;
};

WearSystem::WearSystem(World& world) : m_world(world) {}

void WearSystem::update(entt::registry& registry)
{
  auto view = registry.view<ActionWear, const Position>();
  for (const auto entity : view)
  {
    spdlog::debug("HEREE1");
    auto& action_wear = registry.get<ActionWear>(entity);
    const auto& job = action_wear.job;
    const auto& target = job->target;
    const entt::entity item = static_cast<entt::entity>(target.id);
    spdlog::debug("HEREE2");

    if (!registry.valid(item))
    {
      stop_wear(registry, entity, job);
      continue;
    }
    spdlog::debug("HEREE3");

    // Check if target tile is still there
    if (!m_world.spatial_hash.has(item, target.position.x, target.position.y))
    {
      stop_wear(registry, entity, job);
      continue;
    }
    spdlog::debug("HEREE4");

    registry.remove<Position>(item);
    spdlog::debug("HEREE5");
    registry.remove<Visibility>(item);

    spdlog::debug("HEREE6");
    auto& weared_items = registry.get<WearedItems>(entity);
    spdlog::debug("HEREE9");
    weared_items.items.push_back(item);

    stop_wear(registry, entity, job);
  }
}

}  // namespace dl
