#include "./harvest.hpp"

#include <spdlog/spdlog.h>

#include <entt/entity/registry.hpp>

#include "ecs/components/action_harvest.hpp"
#include "ecs/components/pickable.hpp"
#include "ecs/components/society_agent.hpp"
#include "ecs/components/velocity.hpp"
#include "ecs/components/visibility.hpp"
#include "graphics/constants.hpp"
#include "graphics/frame_data_types.hpp"
#include "world/world.hpp"

namespace dl
{
auto stop_harvesting = [](entt::registry& registry, const entt::entity entity, SocietyAgent& agent) {
  registry.remove<ActionHarvest>(entity);
  agent.state = SocietyAgent::State::Idle;
  const auto& current_job = agent.jobs.top();
  current_job.status = JobStatus::Finished;
};

HarvestSystem::HarvestSystem(World& world) : m_world(world) {}

void HarvestSystem::update(entt::registry& registry, const double delta)
{
  auto view = registry.view<SocietyAgent, ActionHarvest, const Position>();
  for (const auto entity : view)
  {
    auto& action_harvest = registry.get<ActionHarvest>(entity);
    auto& agent = registry.get<SocietyAgent>(entity);

    if (!action_harvest.target)
    {
      /* action_harvest.target = m_world.search_by_flag(tile_flag::harvestable, position.x, position.y, position.z); */

      /* if (!action_harvest.target) */
      /* { */
      stop_harvesting(registry, entity, agent);
      continue;
      /* } */
    }

    // Harvest
    action_harvest.time_left -= delta;

    if (action_harvest.time_left < 0.0)
    {
      // Check if target tile is still there
      auto& target = action_harvest.target;
      const auto& tile = m_world.get(target.x, target.y, target.z);

      if (tile.id != target.id)
      {
        stop_harvesting(registry, entity, agent);
        continue;
      }

      /* const auto& tile_data = m_world.get_tile_data(target.id); */
      /* const auto& action = tile_data.actions.at("harvest"); */

      /* m_world.replace(target.id, action.turns_into, target.x, target.y, target.z); */

      /* // Tile doesn't have any drop */
      /* if (action.gives.empty()) */
      /* { */
      /*   stop_harvesting(registry, entity, agent); */
      /*   continue; */
      /* } */

      /* for (const auto& item : action.gives) */
      /* { */
      /*   const auto drop = registry.create(); */
      /*   registry.emplace<Position>(drop, target.x, target.y, target.z); */
      /*   registry.emplace<Visibility>(drop, */
      /*                                m_world.get_texture_id(), */
      /*                                item.first, */
      /*                                frame_data_type::item, */
      /*                                target.z + renderer::layer_z_offset_items); */
      /*   registry.emplace<Pickable>(drop, item.first); */
      /* } */

      stop_harvesting(registry, entity, agent);
    }
  }
}

}  // namespace dl
