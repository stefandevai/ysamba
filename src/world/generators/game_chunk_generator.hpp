#pragma once

#include <vector>

#include "./island_data.hpp"
#include "core/json.hpp"
#include "core/maths/vector.hpp"
#include "world/tilemap.hpp"

namespace dl
{
struct Chunk;

class GameChunkGenerator
{
 public:
  Vector3i size{1, 1, 1};
  std::vector<float> silhouette_map;
  std::vector<float> mountain_map;
  // std::vector<uint8_t> height_map;
  // std::vector<float> raw_height_map;
  std::vector<float> vegetation_type;
  std::vector<float> vegetation_density;
  std::unique_ptr<Chunk> chunk = nullptr;
  IslandNoiseParams island_params{};

  // Quantity of tiles per map texture pixel
  float map_to_tiles = 20.0f;

  GameChunkGenerator();
  GameChunkGenerator(const Vector3i& size);

  void generate(const int seed, const Vector3i& offset = Vector3i{});
  void set_size(const Vector3i& size);

 private:
  enum Edge
  {
    DL_EDGE_NONE = 0,
    DL_EDGE_TOP = 1,
    DL_EDGE_RIGHT = 2,
    DL_EDGE_BOTTOM = 4,
    DL_EDGE_LEFT = 8,
    DL_EDGE_TOP_LEFT = 16,
    DL_EDGE_TOP_RIGHT = 32,
    DL_EDGE_BOTTOM_RIGHT = 64,
    DL_EDGE_BOTTOM_LEFT = 128,
  };

  // JSON m_json{"./data/world/tile_rules.json"};
  int m_generation_padding = 1;
  Vector3i m_padded_size{size.x + m_generation_padding * 2, size.y + m_generation_padding * 2, 1};
  void m_get_height_map(const int seed, const Vector3i& offset);
  void m_select_tile(const std::vector<int>& terrain, const int x, const int y, const int z);
  int m_select_decoration(const int terrain_id, const int x, const int y, const int z);
  uint32_t m_get_bitmask_4_sided(
      const std::vector<int>& terrain, const int x, const int y, const int z, const int neighbor);
  uint32_t m_get_bitmask_8_sided(
      const std::vector<int>& terrain, const int x, const int y, const int z, const int neighbor, const int source);
  bool m_has_neighbor(const std::vector<int>& terrain, const int x, const int y, const int z, const int neighbor);
};
}  // namespace dl
