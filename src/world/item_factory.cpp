#include "./item_factory.hpp"

#include <spdlog/spdlog.h>

#include <entt/core/hashed_string.hpp>
#include <entt/core/type_traits.hpp>
#include <entt/entity/registry.hpp>

#include "ecs/components/container.hpp"
#include "ecs/components/item.hpp"
#include "ecs/components/item_stack.hpp"
#include "world/world.hpp"

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
  if (item_data.flags.contains("CONTAINER"))
  {
    registry.emplace<Container>(item,
                                item_data.container.weight_capacity,
                                item_data.container.volume_capacity,
                                0.0,
                                0.0,
                                item_data.container.matter_states);
  }
  if (item_data.flags.contains("STACKABLE"))
  {
    registry.emplace<ItemStack>(item);
  }

  registry.emplace<Item>(item, item_data.id);
  return item;
}

double parse_weight(const std::string& raw_weight)
{
  std::string::size_type sz;
  double weight = std::stod(raw_weight, &sz);

  const std::string_view raw_weight_sv = std::string_view(raw_weight);
  const std::string_view weight_unit = raw_weight_sv.substr(sz, raw_weight_sv.size() - 1);

  if (weight_unit == "kg")
  {
    weight *= 1000;
  }
  else if (weight_unit == "mg")
  {
    weight /= 1000;
  }

  return weight;
}

double parse_volume(const std::string& raw_volume)
{
  std::string::size_type sz;
  double volume = std::stod(raw_volume, &sz);

  const std::string_view raw_volume_sv = std::string_view(raw_volume);
  const std::string_view volume_unit = raw_volume_sv.substr(sz, raw_volume_sv.size() - 1);

  if (volume_unit == "L")
  {
    volume *= 1000;
  }

  return volume;
}

std::string weight_to_string(const double weight)
{
  double weight_number = weight;

  if (weight_number < 1.0)
  {
    return std::to_string(weight_number * 1000) + "mg";
  }
  else if (weight_number > 1000.0)
  {
    return std::to_string(weight_number / 1000) + "kg";
  }

  return std::to_string(weight_number) + "g";
}

std::string volume_to_string(const double volume)
{
  double volume_number = volume;

  if (volume_number > 1000.0)
  {
    return std::to_string(volume_number / 1000) + "L";
  }

  return std::to_string(volume_number) + "ml";
}

// Gets a formatted item label for inventory
std::string get_item_label(World& world, entt::registry& registry, entt::entity entity)
{
  const auto& item = registry.get<Item>(entity);
  const auto& item_data = world.get_item_data(item.id);
  uint32_t quantity = 1;

  if (registry.all_of<ItemStack>(entity))
  {
    const auto& item_stack = registry.get<ItemStack>(entity);
    quantity = item_stack.quantity;
  }

  if (quantity > 1)
  {
    return fmt::format("{} ({})", item_data.name, quantity);
  }

  return item_data.name;
}
}  // namespace dl::item_factory
