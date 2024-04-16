#pragma once

#include <queue>
#include <vector>

#include "./island_data.hpp"
#include "core/maths/vector.hpp"
#include "world/tilemap.hpp"

namespace dl
{
class IslandGenerator
{
 public:
  Vector3i size{1, 1, 1};
  std::vector<uint8_t> height_map;
  std::vector<float> silhouette_map;
  std::vector<float> mountain_map;
  std::vector<float> control_map;
  std::vector<float> sea_distance_map;
  std::vector<int> island_mask;
  IslandNoiseParams island_params{};

  IslandGenerator();
  IslandGenerator(const Vector3i& size);

  void generate(const int seed);
  void set_size(const Vector3i& size);

 private:
  const std::string m_default_params_filepath{"./data/world/map_generators/island.json"};

  void m_load_params(const std::string& filepath);
  void m_get_height_map(const int seed);
  void m_flood_fill(std::vector<int>& grid, int x, int y, int value_to_fill);
  bool m_valid_coordinate(int x, int y);
  std::vector<IslandData> m_get_islands(std::vector<int>& grid, uint32_t islands_to_keep);
  IslandData m_get_island(const std::vector<int>& grid, std::vector<int>& mask, int x, int y);
};
}  // namespace dl
