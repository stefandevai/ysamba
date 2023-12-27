#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "core/json.hpp"
#include "core/maths/vector.hpp"
#include "world/cell.hpp"
#include "world/tilemap.hpp"

namespace dl
{
class MapGenerator
{
 public:
  int width = 1;
  int height = 1;
  int depth = 1;
  std::vector<Cell> tiles;
  std::vector<int> height_map;

  MapGenerator() = default;
  MapGenerator(const int width, const int height, const int depth = 32) : width(width), height(height), depth(depth) {}

  void generate(const int seed);
  void set_size(const Vector3i& size);

 private:
  JSON m_json{"./data/world/map_generators/terrain.json"};

  void m_get_height_map(std::vector<double>& height_values, const int seed);
  float m_get_rectangle_gradient_value(const int x, const int y);
};
}  // namespace dl
