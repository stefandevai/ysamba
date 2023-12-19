#pragma once

#include <glm/glm.hpp>
#include <queue>
#include <vector>

#include "./island_data.hpp"
#include "./lib/spline.hpp"
#include "./river_segment.hpp"
#include "core/lua_api.hpp"
#include "world/point.hpp"
#include "world/tilemap.hpp"

namespace dl
{
struct BayData;

class CompareVectorSizes
{
 public:
  bool operator()(const IslandData& a, const IslandData& b) const { return a.points.size() > b.points.size(); }
};

template <class T>
class ComparePointsX
{
 public:
  bool operator()(const std::shared_ptr<Point<T>>& a, const std::shared_ptr<Point<T>>& b) const { return a->x < b->x; }
};

struct RiverData
{
  tk::spline spline_x;
  tk::spline spline_y;
  std::vector<double> points_t;
  double max_length;
};

using IslandQueue = std::priority_queue<IslandData, std::vector<IslandData>, CompareVectorSizes>;

class ErosionGenerator
{
 public:
  ErosionGenerator(const int width, const int height) : m_width(width), m_height(height) {}

  std::vector<double> generate(const int seed);

 private:
  LuaAPI m_lua = LuaAPI{"generators/terrain.lua"};
  const int m_width;
  const int m_height;

  void m_generate_silhouette(std::vector<double>& tiles, const int seed);
  void m_erode(std::vector<double>& tiles, const uint32_t cycles);
  glm::vec3 m_surface_normal(const std::vector<double>& height_map, int i, int j);
  float m_get_rectangle_gradient_value(const int x, const int y);
  std::vector<IslandData> m_get_islands(std::vector<int>& tiles);
  IslandQueue m_get_island_queue(const std::vector<int>& tiles);
  IslandData m_get_island(const std::vector<int>& tiles, std::vector<int>& mask, const int x, const int y);
  bool m_valid_coord(const int x, const int y);
  bool m_valid_point(const Point<int>& point);
  std::vector<Point<int>> m_get_coastline(const std::vector<int>& tiles, const IslandData& island);
  Point<int> m_get_first_coastline_point(const std::vector<int>& tiles, const IslandData& island);
  int m_get_point_value(const Point<int>& point, const std::vector<int>& tiles);
  bool m_is_coast_point(const Point<int>& point, const std::vector<int>& tiles);
  bool m_is_water(const Point<int>& point, const std::vector<int>& tiles);
  bool m_contains_island_area(const Point<int>& point,
                              const float segment_width_h,
                              const float segment_width_v,
                              const std::vector<int>& island_mask);
  void m_flood_fill(const int value, const int x, const int y, std::vector<int>& tiles);
  std::vector<Point<int>> m_get_bays(std::vector<Point<int>>& coastline, const IslandData& island);
  bool m_has_land_intersection(const Point<int>& point_a,
                               const Point<int>& point_b,
                               const std::vector<int>& tiles,
                               const int water_value);
  BayData m_get_bay_data(const Point<int>& point_a,
                         const Point<int>& point_b,
                         const std::vector<int>& tiles,
                         const int minimum,
                         std::vector<int>& mask,
                         const int water_value);
  void m_build_island_structure(IslandData& island);
  bool m_center_is_coast(const Point<int>& center, const std::vector<int>& island_mask);
  void m_generate_main_river(IslandData& island,
                             std::vector<Point<int>>& bays,
                             std::vector<int>& tiles,
                             const int seed = 0);
  std::pair<Point<int>, Point<int>> m_get_river_source_and_mouth(IslandData& island,
                                                                 std::vector<Point<int>>& bays,
                                                                 const int seed);
  RiverData m_get_river_data(const Point<int>& source, const Point<int>& mouth);
  void m_create_meanders(RiverData& river, const Point<int>& source, const Point<int>& mouth, std::vector<int>& tiles);
  float m_distance(const Point<int>& point_a, const Point<int>& point_b);
  double m_distance(const Point<double>& point_a, const Point<double>& point_b);
  double m_menger_curvature(const Point<double>& point_a,
                            const Point<double>& point_b,
                            const Point<double>& point_c,
                            const double length_1,
                            const double length_2,
                            const double length_3);

  // Debug functions
  void m_draw_point(const Point<int>& point, const int value, std::vector<int>& tiles);
  void m_draw_big_point(const Point<int>& point, const int value, std::vector<int>& tiles);
  void m_draw_line(const Point<int>& origin, const Point<int>& destination, const int value, std::vector<int>& tiles);
};
}  // namespace dl
