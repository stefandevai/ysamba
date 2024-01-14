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
  };

  JSON m_json{"./data/world/map_generators/terrain.json"};
  int m_generation_padding = 1;
  void m_get_height_map(std::vector<float>& height_values, const int seed, const Vector3i& offset);
  float m_get_rectangle_gradient_value(const int x, const int y);
  void m_select_tile(const std::vector<int>& terrain, const int x, const int y, const int z);
  uint32_t m_get_bitmask(const std::vector<int>& terrain, const int x, const int y, const int z);
};
}  // namespace dl
