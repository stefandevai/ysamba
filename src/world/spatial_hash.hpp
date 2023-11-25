#pragma once

#include <entt/entity/registry.hpp>
#include <unordered_map>
#include <vector>

#include "ecs/components/position.hpp"
#include "ecs/components/visibility.hpp"

namespace dl
{
class SpatialHash
{
 public:
  SpatialHash();
  SpatialHash(const uint32_t width, const uint32_t height, const uint32_t cell_dimension);

  void load(const uint32_t width, const uint32_t height, const uint32_t cell_dimension);
  uint32_t add(const entt::entity object, const int x, const int y);
  void remove(const entt::entity object, const uint32_t key);
  uint32_t update(const entt::entity object, const int x, const int y, const uint32_t key);
  std::vector<entt::entity> get(const int x, const int y);

  template <typename... T>
  [[nodiscard]] entt::entity search_by_component(const uint32_t x, const uint32_t y, const entt::registry& registry)
  {
    const auto& entities = get(x, y);

    entt::entity candidate = entt::null;
    size_t candidate_z = 0;

    for (const auto entity : entities)
    {
      if (!registry.all_of<Position, Visibility, T...>(entity))
      {
        continue;
      }

      const auto& position = registry.get<Position>(entity);
      const auto& visibility = registry.get<Visibility>(entity);
      const auto rounded_x = std::round(position.x);
      const auto rounded_y = std::round(position.y);

      if (visibility.layer_z >= candidate_z && x == rounded_x && y == rounded_y)
      {
        candidate = entity;
        candidate_z = visibility.layer_z;
      }
    }

    return candidate;
  }

 private:
  using HashMap = std::unordered_multimap<uint32_t, entt::entity>;
  HashMap m_hash;
  uint32_t m_width;
  uint32_t m_height;
  uint32_t m_cell_dimension;
  uint32_t m_number_of_cells;
  uint32_t m_horizontal_cells;
  uint32_t m_vertical_cells;

  uint32_t m_get_key(const int x, const int y);
  std::vector<uint32_t> m_get_search_keys(const int x, const int y);
};
}  // namespace dl
