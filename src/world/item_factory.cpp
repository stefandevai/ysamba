#include "./item_factory.hpp"

#include <entt/core/hashed_string.hpp>
#include <entt/core/type_traits.hpp>
#include <entt/entity/registry.hpp>

#include "ecs/components/item.hpp"

namespace dl::item_factory
{
entt::entity create(const ItemData& item_data, entt::registry& registry)
{
  using namespace entt::literals;

  entt::entity item = registry.create();

  if (item_data.flags.contains("WEARABLE"))
  {
    registry.emplace<entt::tag<"wearable"_hs>>(item);
  }
  if (item_data.flags.contains("PICKABLE"))
  {
    registry.emplace<entt::tag<"pickable"_hs>>(item);
  }
  if (item_data.flags.contains("WIELDABLE"))
  {
    registry.emplace<entt::tag<"wieldable"_hs>>(item);
  }
  if (item_data.flags.contains("FLAMMABLE"))
  {
    registry.emplace<entt::tag<"flammable"_hs>>(item);
  }

  registry.emplace<Item>(item, item_data.id);
  return item;
}
}  // namespace dl::item_factory
