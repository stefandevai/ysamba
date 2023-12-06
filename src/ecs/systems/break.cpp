#include "./break.hpp"

#include <spdlog/spdlog.h>

#include <entt/entity/registry.hpp>

#include "ecs/components/action_break.hpp"
#include "ecs/components/pickable.hpp"
#include "ecs/components/society_agent.hpp"
#include "ecs/components/velocity.hpp"
#include "ecs/components/visibility.hpp"
#include "graphics/constants.hpp"
#include "graphics/frame_data_types.hpp"
#include "world/tile_flag.hpp"
#include "world/world.hpp"

namespace dl
{
auto stop_breaking = [](entt::registry& registry, const entt::entity entity, SocietyAgent& agent) {
  registry.remove<ActionBreak>(entity);
  agent.state = SocietyAgent::State::Idle;
  agent.jobs.pop();
};

BreakSystem::BreakSystem(World& world) : m_world(world) {}

void BreakSystem::update(entt::registry& registry, const double delta)
{
  auto view = registry.view<SocietyAgent, ActionBreak, const Position>();
  for (const auto entity : view)
  {
    auto& action_break = registry.get<ActionBreak>(entity);
    auto& agent = registry.get<SocietyAgent>(entity);

    if (!action_break.target)
    {
      stop_breaking(registry, entity, agent);
      continue;
    }

    action_break.time_left -= delta;

    if (action_break.time_left < 0.0)
    {
      // Check if target tile is still there
      auto& target = action_break.target;
      const auto& tile = m_world.get(target.x, target.y, target.z);

      if (tile.id != target.id)
      {
        stop_breaking(registry, entity, agent);
        continue;
      }

      const auto& tile_data = m_world.get_tile_data(target.id);
      const auto& action = tile_data.actions.at("break");

      m_world.replace(target.id, action.turns_into, target.x, target.y, target.z);

      // Tile doesn't have any drop
      if (action.gives.empty())
      {
        stop_breaking(registry, entity, agent);
        continue;
      }

      for (const auto& item : action.gives)
      {
        const auto drop = registry.create();
        registry.emplace<Position>(drop, target.x, target.y, target.z);
        registry.emplace<Visibility>(drop,
                                     m_world.get_texture_id(),
                                     item.first,
                                     frame_data_type::item,
                                     target.z + renderer::layer_z_offset_items);
        registry.emplace<Pickable>(drop, item.first);
      }

      stop_breaking(registry, entity, agent);
    }
  }
}

}  // namespace dl
