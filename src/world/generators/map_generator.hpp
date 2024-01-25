#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "core/json.hpp"
#include "core/maths/vector.hpp"
#include "world/cell.hpp"
#include "world/tilemap.hpp"

namespace dl
{
struct Chunk;

class MapGenerator
{
 public:
  int width = 1;
  int height = 1;
  int depth = 1;
  // std::vector<Cell> tiles;
  // std::vector<int> height_map;
  std::vector<float> raw_height_map;
  std::vector<float> vegetation_type;
  std::vector<float> vegetation_density;
  std::unique_ptr<Chunk> chunk = nullptr;

  MapGenerator() = default;
  MapGenerator(const int width, const int height, const int depth = 32) : width(width), height(height), depth(depth) {}

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

  JSON m_json{"./data/world/tile_rules.json"};
  int m_generation_padding = 1;
  void m_get_height_map(const int seed, const Vector3i& offset);
  float m_get_rectangle_gradient_value(const int x, const int y);
  void m_select_tile(const std::vector<int>& terrain, const int x, const int y, const int z);
  int m_select_decoration(const int terrain_id, const int x, const int y, const int z);
  uint32_t m_get_bitmask_4_sided(
      const std::vector<int>& terrain, const int x, const int y, const int z, const int neighbor);
  uint32_t m_get_bitmask_8_sided(
      const std::vector<int>& terrain, const int x, const int y, const int z, const int neighbor, const int source);
  bool m_has_neighbor(const std::vector<int>& terrain, const int x, const int y, const int z, const int neighbor);
};
}  // namespace dl
