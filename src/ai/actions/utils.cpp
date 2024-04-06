#include "./utils.hpp"

#include <entt/entity/registry.hpp>

#include "ai/actions/walk.hpp"
#include "config.hpp"
#include "core/maths/vector.hpp"
#include "ecs/components/carried_items.hpp"
#include "ecs/components/item.hpp"
#include "ecs/components/job_data.hpp"
#include "ecs/components/society_agent.hpp"
#include "ecs/components/weared_items.hpp"
#include "ecs/components/wielded_items.hpp"
#include "world/world.hpp"

namespace dl::utils
{
bool has_consumables(entt::registry& registry, const std::map<uint32_t, uint32_t>& consumables)
{
  // TODO: Check society inventory for consumable items
  (void)consumables;
  (void)registry;
  return true;
}

bool validate_tile_action(ValidateTileActionParams params)
{
  const auto& tile = params.world.get(params.position);

  if (!tile.actions.contains(params.job_type))
  {
    return false;
  }

  const auto& consumed_items = tile.actions.at(params.job_type).consumes;

  if (!consumed_items.empty())
  {
    if (!has_consumables(params.registry, consumed_items))
    {
      return false;
    }
  }

  return true;
}

bool has_qualities_required(HasQualitiesRequiredParams params)
{
  const auto& carried_items = params.registry.get<CarriedItems>(params.entity);
  const auto& weared_items = params.registry.get<WearedItems>(params.entity);
  const auto& wielded_items = params.registry.get<WieldedItems>(params.entity);

  for (const auto& quality : params.qualities_required)
  {
    bool has_quality = false;

    for (const auto item_entity : carried_items.items)
    {
      const auto& item = params.registry.get<Item>(item_entity);
      const auto& item_data = params.world.get_item_data(item.id);

      if (item_data.qualities.contains(quality))
      {
        has_quality = true;
        continue;
      }
    }

    if (!has_quality)
    {
      for (const auto item_entity : weared_items.items)
      {
        const auto& item = params.registry.get<Item>(item_entity);
        const auto& item_data = params.world.get_item_data(item.id);

        if (item_data.qualities.contains(quality))
        {
          has_quality = true;
          continue;
        }
      }
    }

    if (!has_quality)
    {
      if (params.registry.valid(wielded_items.left_hand))
      {
        const auto& item = params.registry.get<Item>(wielded_items.left_hand);
        const auto& item_data = params.world.get_item_data(item.id);

        if (item_data.qualities.contains(quality))
        {
          has_quality = true;
        }
      }

      if (!has_quality && params.registry.valid(wielded_items.right_hand))
      {
        const auto& item = params.registry.get<Item>(wielded_items.right_hand);
        const auto& item_data = params.world.get_item_data(item.id);

        if (item_data.qualities.contains(quality))
        {
          has_quality = true;
        }
      }
    }

    if (!has_quality)
    {
      return false;
    }
  }
  return true;
}

bool create_tile_job(CreateTileJobParams params)
{
  if (!validate_tile_action({.registry = params.registry,
                             .world = params.world,
                             .position = params.position,
                             .job_type = params.job_type}))
  {
    // TODO: Notify player that validation failed for action
    return false;
  }

  const auto& tile = params.world.get(params.position);
  const auto& qualities_required = tile.actions.at(params.job_type).qualities_required;
  bool job_assigned = false;

  const auto assign_job = [&params, &tile, &qualities_required, &job_assigned](const entt::entity entity) {
    // Check if the agent has the necessary qualities to perform the action
    if (!qualities_required.empty())
    {
      if (!utils::has_qualities_required({.world = params.world,
                                          .registry = params.registry,
                                          .entity = entity,
                                          .qualities_required = qualities_required}))
      {
        // TODO: Notify player that items with required qualities are needed
        return;
      }
    }

    auto& agent = params.registry.get<SocietyAgent>(entity);
    const auto job = params.registry.create();

    params.registry.emplace<Target>(job, params.position, tile.id);
    params.registry.emplace<JobData>(job, params.job_type);

    // TODO: Create walk job if needed on the respective action system
    if (params.job_type != JobType::Walk)
    {
      action::walk::create_job(
          {.registry = params.registry, .agent_entity = entity, .agent = &agent, .position = params.position});
    }

    agent.jobs.push(Job{config::ai::default_job_priority, job});
    job_assigned = true;
  };

  std::for_each(params.entities.begin(), params.entities.end(), assign_job);

  return job_assigned;
}

bool create_item_job(CreateItemJobParams params)
{
  assert(params.item != entt::null && "No item provided for job");

  bool job_assigned = false;

  const auto assign_job = [&params, &job_assigned](const entt::entity entity) {
    const auto job = params.registry.create();
    params.registry.emplace<Target>(job, params.position, static_cast<uint32_t>(params.item));
    params.registry.emplace<JobData>(job, params.job_type);

    action::walk::create_job({.registry = params.registry, .agent_entity = entity, .position = params.position});

    auto& agent = params.registry.get<SocietyAgent>(entity);
    agent.jobs.push(Job{config::ai::default_job_priority, job});

    job_assigned = true;
  };

  std::for_each(params.entities.begin(), params.entities.end(), assign_job);

  return job_assigned;
}
}  // namespace dl::utils
