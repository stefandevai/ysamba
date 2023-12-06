#include "./dig.hpp"

#include <spdlog/spdlog.h>

#include <entt/entity/registry.hpp>

#include "ecs/components/action_dig.hpp"
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
auto stop_digging = [](entt::registry& registry, const entt::entity entity, SocietyAgent& agent) {
  registry.remove<ActionDig>(entity);
  agent.state = SocietyAgent::State::Idle;
  agent.jobs.pop();
};

DigSystem::DigSystem(World& world) : m_world(world) {}

void DigSystem::update(entt::registry& registry, const double delta)
{
  auto view = registry.view<SocietyAgent, ActionDig, const Position>();
  for (const auto entity : view)
  {
    auto& action_dig = registry.get<ActionDig>(entity);
    auto& agent = registry.get<SocietyAgent>(entity);

    if (!action_dig.target)
    {
      stop_digging(registry, entity, agent);
      continue;
    }

    action_dig.time_left -= delta;

    if (action_dig.time_left < 0.0)
    {
      // Check if target tile is still there
      auto& target = action_dig.target;
      const auto& tile = m_world.get(target.x, target.y, target.z);

      if (tile.id != target.id)
      {
        stop_digging(registry, entity, agent);
        continue;
      }

      const auto& tile_data = m_world.get_tile_data(target.id);

      // Tile doesn't have any drop
      if (tile_data.drop_ids.empty())
      {
        stop_digging(registry, entity, agent);
        continue;
      }

      m_world.replace(target.id, tile_data.after_removed, target.x, target.y, target.z);

      const auto& position = registry.get<Position>(entity);

      for (const auto id : tile_data.drop_ids)
      {
        const auto drop = registry.create();
        registry.emplace<Position>(drop, position.x, position.y, position.z);
        registry.emplace<Visibility>(
            drop, m_world.get_texture_id(), id, frame_data_type::item, target.z + renderer::layer_z_offset_items);
        registry.emplace<Pickable>(drop, id);
      }

      stop_digging(registry, entity, agent);
    }
  }
}

}  // namespace dl
