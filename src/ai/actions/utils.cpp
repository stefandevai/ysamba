#include "./utils.hpp"

#include <spdlog/spdlog.h>

#include <entt/entity/registry.hpp>

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
}  // namespace dl::utils
