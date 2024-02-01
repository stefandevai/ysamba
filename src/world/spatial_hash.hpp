#pragma once

#include <array>
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
  using TestFunction = std::function<bool(const entt::entity)>;

  SpatialHash();
  SpatialHash(const uint32_t cell_dimension);

  void load(const uint32_t cell_dimension);
  int add(const entt::entity object, const int x, const int y, const int z);
  void remove(const entt::entity object, const int key);
  int update(const entt::entity object, const int x, const int y, const int z, const int key);
  [[nodiscard]] std::vector<entt::entity> get(const int x, const int y, const int z);
  [[nodiscard]] bool has(entt::entity entity, const int x, const int y, const int z);
  [[nodiscard]] std::vector<entt::entity> get_if(const Vector3i& position, TestFunction test_function);

  /* [[nodiscard]] std::vector<entt::entity> get_if(const uint32_t x, const uint32_t y, const entt::registry& registry)
   */

  template <typename... T>
  [[nodiscard]] entt::entity get_by_component(const int x, const int y, const int z, const entt::registry& registry)
  {
    const auto& entities = get(x, y, z);

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
  using HashMap = std::unordered_multimap<int, entt::entity>;
  HashMap m_hash;
  uint32_t m_cell_dimension;

  int m_get_key(const int x, const int y, const int z);
  std::array<int, 27> m_get_search_keys(const int x, const int y, const int z);
};
}  // namespace dl
