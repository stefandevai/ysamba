#pragma once

#include <vector>

#include "core/maths/vector.hpp"
#include "world/generators/biome_type.hpp"
#include "world/generators/island_data.hpp"

namespace dl
{
class IslandGenerator
{
 public:
  Vector3i size{1, 1, 1};
  std::vector<BiomeType> biome_map;
  std::vector<float> height_map;
  std::vector<float> humidity_map;
  std::vector<float> temperature_map;
  std::vector<float> sea_distance_field;
  std::vector<int> island_mask;
  IslandNoiseParams island_params{};

  IslandGenerator(const Vector3i& size);

  void generate(const int seed);
  void set_size(const Vector3i& size);

 private:
  const std::string m_default_params_filepath{"./data/world/map_generators/island.json"};

  void m_load_params(const std::string& filepath);
  void m_compute_maps(const int seed);
  void m_generate_biomes();
  void m_flood_fill(std::vector<int>& grid, int x, int y, int value_to_fill);
  bool m_valid_coordinate(int x, int y);
  std::vector<IslandData> m_get_islands(std::vector<int>& grid, uint32_t islands_to_keep);
  IslandData m_get_island(const std::vector<int>& grid, std::vector<int>& mask, int x, int y);
};
}  // namespace dl
