#pragma once

#include <queue>
#include <vector>

#include "./island_data.hpp"
#include "core/maths/vector.hpp"
#include "world/tilemap.hpp"

namespace dl
{
struct IslandMaskParams
{
  int octaves = 4;
  float frequency = 0.005f;
  float lacunarity = 3.2f;
  float gain = 0.32f;
  float weighted_strength = 0.25f;
  float tier_land = 0.002f;
  float distance_influence = 3.0f;
  float distance_k = 2.0f;
  float display_mask = true;
  std::string noise_string{};
};

struct CompareVectorSizes
{
  bool operator()(const IslandData& a, const IslandData& b) const { return a.points.size() > b.points.size(); }
};

using IslandQueue = std::priority_queue<IslandData, std::vector<IslandData>, CompareVectorSizes>;

class IslandGenerator
{
 public:
  int width = 1;
  int height = 1;
  int depth = 1;
  std::vector<float> raw_height_map;
  std::vector<int> island_mask;
  IslandMaskParams island_params{};

  IslandGenerator();
  IslandGenerator(const int width, const int height, const int depth);

  void generate(const int seed);
  void set_size(const Vector3i& size);

 private:
  const std::string m_default_params_filepath{"./data/world/map_generators/island.json"};

  void m_load_params(const std::string& filepath);
  void m_get_height_map(const int seed);
  double m_get_rectangle_gradient_value(const int x, const int y);
  void m_flood_fill(const int value, const int x, const int y, std::vector<int>& tiles);
  bool m_valid_coord(const int x, const int y);
  std::vector<IslandData> m_get_islands(std::vector<int>& tiles, const uint32_t islands_to_keep);
  IslandQueue m_get_island_queue(const std::vector<int>& tiles);
  IslandData m_get_island(const std::vector<int>& tiles, std::vector<int>& mask, const int x, const int y);
};
}  // namespace dl
