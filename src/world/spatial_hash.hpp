#pragma once

#include <array>
#include <entt/entity/registry.hpp>
#include <unordered_map>
#include <vector>

#include "ecs/components/position.hpp"
#include "ecs/components/sprite.hpp"

namespace dl
{
class SpatialHash
{
 public:
  using TestFunction = std::function<bool(const entt::entity)>;

  SpatialHash();
  SpatialHash(const uint32_t cell_dimension);

  void load(const uint32_t cell_dimension);
  uint32_t add(const entt::entity object, const int x, const int y, const int z);
  void remove(const entt::entity object, const uint32_t key);
  uint32_t update(const entt::entity object, const int x, const int y, const int z, const uint32_t key);
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

    for (auto it = entities.rbegin(); it != entities.rend(); ++it)
    {
      const auto entity = *it;
      if (!registry.valid(entity))
      {
        continue;
      }
      if (!registry.all_of<Position, T...>(entity))
      {
        continue;
      }

      const auto& position = registry.get<Position>(entity);
      const auto rounded_x = std::round(position.x);
      const auto rounded_y = std::round(position.y);
      const auto rounded_z = std::round(position.z);

      if (z == rounded_z && x == rounded_x && y == rounded_y)
      {
        return entity;
      }
    }

    return entt::null;
  }

  template <typename... T>
  [[nodiscard]] std::vector<entt::entity> get_all_by_component(const int x,
                                                               const int y,
                                                               const int z,
                                                               const entt::registry& registry) const
  {
    const auto& entities = get(x, y, z);
    std::vector<entt::entity> entities_with_components{};

    for (auto it = entities.rbegin(); it != entities.rend(); ++it)
    {
      const auto entity = *it;

      if (!registry.valid(entity))
      {
        continue;
      }
      if (!registry.all_of<Position, T...>(entity))
      {
        continue;
      }

      const auto& position = registry.get<Position>(entity);
      const auto rounded_x = std::round(position.x);
      const auto rounded_y = std::round(position.y);
      const auto rounded_z = std::round(position.z);

      if (z == rounded_z && x == rounded_x && y == rounded_y)
      {
        entities_with_components.push_back(entity);
      }
    }

    return entities_with_components;
  }

 private:
  using HashMap = std::unordered_multimap<uint32_t, entt::entity>;
  HashMap m_hash{};
  uint32_t m_cell_dimension{};

  uint32_t m_get_key(const int x, const int y, const int z) const;
  std::array<uint32_t, 27> m_get_search_keys(const int x, const int y, const int z) const;
};
}  // namespace dl
