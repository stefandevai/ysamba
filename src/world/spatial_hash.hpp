#pragma once

#include <entt/entity/entity.hpp>
#include <unordered_map>
#include <vector>

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
