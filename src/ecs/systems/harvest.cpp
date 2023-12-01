#include "./harvest.hpp"

#include <spdlog/spdlog.h>

#include <entt/entity/registry.hpp>

#include "ecs/components/action_harvest.hpp"
#include "ecs/components/pickable.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/society_agent.hpp"
#include "ecs/components/velocity.hpp"
#include "ecs/components/visibility.hpp"
#include "graphics/constants.hpp"
#include "graphics/frame_data_types.hpp"
#include "world/tile_flag.hpp"
#include "world/world.hpp"

namespace dl
{
auto stop_harvesting = [](entt::registry& registry, const entt::entity entity, SocietyAgent& agent) {
  registry.remove<ActionHarvest>(entity);
  agent.state = SocietyAgent::State::Idle;
};

HarvestSystem::HarvestSystem(World& world) : m_world(world) {}

void HarvestSystem::update(entt::registry& registry, const double delta)
{
  auto view = registry.view<SocietyAgent, ActionHarvest, const Position>();
  for (const auto entity : view)
  {
    auto& action_harvest = registry.get<ActionHarvest>(entity);
    auto& agent = registry.get<SocietyAgent>(entity);
    const auto& position = registry.get<Position>(entity);

    if (!action_harvest.target)
    {
      /* action_harvest.target = m_world.search_by_flag(tile_flag::harvestable, position.x, position.y, position.z); */

      /* if (!action_harvest.target) */
      /* { */
      stop_harvesting(registry, entity, agent);
      continue;
      /* } */
    }

    auto& target = action_harvest.target;

    // If the target tile is not adjacent, move towards the target
    if (std::abs(target.x - std::round(position.x)) > 1 || std::abs(target.y - std::round(position.y)) > 1)
    {
      // If the target path is empty, that means that the target disappeared.
      if (target.path.size() <= 1)
      {
        stop_harvesting(registry, entity, agent);
        continue;
      }
      auto current_target_position = target.path.top();

      if (std::round(position.x) == current_target_position.first &&
          std::round(position.y) == current_target_position.second)
      {
        target.path.pop();
        current_target_position = target.path.top();
      }

      const auto x_dir = current_target_position.first - std::round(position.x);
      const auto y_dir = current_target_position.second - std::round(position.y);

      if (registry.all_of<Velocity>(entity))
      {
        registry.patch<Velocity>(entity, [x_dir, y_dir](auto& velocity) {
          velocity.x = x_dir;
          velocity.y = y_dir;
        });
      }
      else
      {
        registry.emplace<Velocity>(entity, x_dir, y_dir, 0.);
      }
      continue;
    }

    // Harvest
    action_harvest.time_left -= delta;

    if (action_harvest.time_left < 0.0)
    {
      // Check if target tile is still there
      const auto& tile = m_world.get(target.x, target.y, target.z);

      if (tile.id != target.id)
      {
        stop_harvesting(registry, entity, agent);
        continue;
      }

      const auto& tile_data = m_world.get_tile_data(target.id);

      // Tile doesn't have any drop
      if (tile_data.drop_ids.empty())
      {
        stop_harvesting(registry, entity, agent);
        continue;
      }

      m_world.replace(target.id, tile_data.after_removed, target.x, target.y, target.z);

      for (const auto id : tile_data.drop_ids)
      {
        const auto drop = registry.create();
        registry.emplace<Position>(drop, target.x, target.y, target.z);
        registry.emplace<Visibility>(
            drop, m_world.get_texture_id(), id, frame_data_type::item, target.z + renderer::layer_z_offset_items);
        registry.emplace<Pickable>(drop);
      }

      stop_harvesting(registry, entity, agent);
    }
  }
}

}  // namespace dl
