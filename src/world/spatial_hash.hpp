#pragma once

#include <array>
#include <entt/entity/registry.hpp>
#include <unordered_map>
#include <vector>

#include "ecs/components/position.hpp"
#include "graphics/tile_render_data.hpp"

namespace dl
{
class SpatialHash
{
 public:
  using TestFunction = std::function<bool(const entt::entity)>;

  SpatialHash();
  SpatialHash(const uint32_t cell_dimension);

  void load(const uint32_t cell_dimension);
  int add(const entt::entity object, const int x, const int y, const int z);
  void remove(const entt::entity object, const int key);
  int update(const entt::entity object, const int x, const int y, const int z, const int key);
  [[nodiscard]] std::vector<entt::entity> get(const int x, const int y, const int z) const;
  [[nodiscard]] bool has(entt::entity entity, const int x, const int y, const int z) const;
  [[nodiscard]] std::vector<entt::entity> get_if(const Vector3i& position, TestFunction test_function) const;

  template <typename... T>
  [[nodiscard]] entt::entity get_by_component(const int x,
                                              const int y,
                                              const int z,
                                              const entt::registry& registry) const
  {
    const auto& entities = get(x, y, z);

    entt::entity candidate = entt::null;

    for (const auto entity : entities)
    {
      if (!registry.all_of<Position, SpriteRenderData, T...>(entity))
      {
        continue;
      }

      const auto& position = registry.get<Position>(entity);
      const auto rounded_x = std::round(position.x);
      const auto rounded_y = std::round(position.y);
      const auto rounded_z = std::round(position.z);

      if (z == rounded_z && x == rounded_x && y == rounded_y)
      {
        candidate = entity;
      }
    }

    return candidate;
  }

 private:
  using HashMap = std::unordered_multimap<int, entt::entity>;
  HashMap m_hash;
  uint32_t m_cell_dimension;

  int m_get_key(const int x, const int y, const int z) const;
  std::array<int, 27> m_get_search_keys(const int x, const int y, const int z) const;
};
}  // namespace dl
