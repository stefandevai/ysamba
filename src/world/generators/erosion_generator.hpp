#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "core/lua_api.hpp"
#include "world/tilemap.hpp"

namespace dl
{
class ErosionGenerator
{
 public:
  ErosionGenerator(const int width, const int height, const int z_levels = 10)
      : m_width(width), m_height(height), m_z_levels(z_levels)
  {
  }

  std::vector<int> generate(const int seed);

 private:
  LuaAPI m_lua = LuaAPI{"generators/terrain.lua"};
  const int m_width;
  const int m_height;
  const int m_z_levels;

  void m_generate_silhouette(std::vector<double>& tiles, const int seed);
  float m_get_rectangle_gradient_value(const int x, const int y);
  void m_erode(std::vector<double>& tiles, const uint32_t cycles);
  glm::vec3 m_surface_normal(const std::vector<double>& height_map, int i, int j);
};
}  // namespace dl
