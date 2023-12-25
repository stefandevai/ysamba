#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "core/json.hpp"
#include "world/cell.hpp"
#include "world/tilemap.hpp"

namespace dl
{
class MapGenerator
{
 private:
  const int m_width;
  const int m_height;
  const int m_z_levels;

 public:
  std::vector<Cell> tiles = std::vector<Cell>(m_width * m_height * m_z_levels);
  std::vector<int> height_map = std::vector<int>(m_width * m_height);

  MapGenerator(const int width, const int height, const int z_levels = 10)
      : m_width(width), m_height(height), m_z_levels(z_levels)
  {
  }

  void generate(const int seed);

 private:
  JSON m_json{"./data/world/map_generators/terrain.json"};

  void m_get_height_map(std::vector<double>& height_values, const int seed);
  float m_get_rectangle_gradient_value(const int x, const int y);
};
}  // namespace dl
