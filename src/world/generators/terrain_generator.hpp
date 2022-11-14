#pragma once

#include <vector>
#include <queue>
#include "../../core/lua_api.hpp"
#include "../point.hpp"
#include "./lib/fast_noise_lite.hpp"


namespace dl
{
  class CompareVectorSizes
  {
    public:
      bool operator() (const std::vector<Point<unsigned int>>& a, const std::vector<Point<unsigned int>>& b) const
      {
        return a.size() > b.size();
      }
  };

  /* struct GeometryPoint<unsigned int> */
  /* { */
  /*   explicit GeometryPoint<unsigned int>(const Point<unsigned int>& point) : point(point) {} */

  /*   Point<unsigned int> point; */

  /*   GeometryPoint<unsigned int>* left = nullptr; */
  /*   GeometryPoint<unsigned int>* right = nullptr; */
  /*   GeometryPoint<unsigned int>* bottom = nullptr; */
  /*   GeometryPoint<unsigned int>* top = nullptr; */
  /*   GeometryPoint<unsigned int>* top_left = nullptr; */
  /*   GeometryPoint<unsigned int>* top_right = nullptr; */
  /*   GeometryPoint<unsigned int>* bottom_left = nullptr; */
  /*   GeometryPoint<unsigned int>* bottom_right = nullptr; */
  /* }; */

  struct BayData
  {
    explicit BayData(const int area, const Point<unsigned int>& point) : area(area), point(point) {}
    int area = 0;
    Point<unsigned int> point = Point<unsigned int>(0, 0);
  };

  typedef std::priority_queue<std::vector<Point<unsigned int>>, std::vector<std::vector<Point<unsigned int>>>, CompareVectorSizes> island_queue;

  class TerrainGenerator
  {
    public:
      static std::vector<int> generate(const uint32_t width, const uint32_t height, const int seed);

    private:
      static LuaAPI m_lua;
      static FastNoiseLite m_noise;

      static void m_generate_silhouette(std::vector<int>& tiles, const uint32_t width, const uint32_t height, const int seed);
      static float m_get_rectangle_gradient_value(const int x, const int y, const int width, const int height);
      static std::vector<std::vector<Point<unsigned int>>> m_adjust_and_get_islands(std::vector<int>& tiles, const int width, const int height);
      static island_queue m_get_islands(const std::vector<int>& tile, const uint32_t width, const uint32_t heights);
      static std::vector<Point<unsigned int>> m_get_island(const std::vector<int>& tiles, std::vector<int>& mask, const uint32_t x, const uint32_t y, const uint32_t width, const uint32_t height);
      static bool m_valid_coord(const uint32_t x, const uint32_t y, const uint32_t width, const uint32_t height);
      static bool m_valid_point(const Point<unsigned int>& point, const uint32_t width, const uint32_t height);
      static std::vector<Point<unsigned int>> m_get_coastline(const std::vector<int>& tiles, const uint32_t width, const uint32_t height, const std::vector<Point<unsigned int>>& main_island);
      /* static std::vector<GeometryPoint<unsigned int>> m_get_island_geometry(const uint32_t width, const uint32_t height, const std::vector<Point<unsigned int>>& island); */
      static std::vector<Point<unsigned int>> m_get_island_structure(const uint32_t width, const uint32_t height, const std::vector<Point<unsigned int>>& island, std::vector<int>& tiles);
      static Point<unsigned int> m_get_first_coastline_point(const std::vector<int>& tiles, const uint32_t width, const uint32_t height, const std::vector<Point<unsigned int>>& main_island);
      static int m_get_point_value(const Point<unsigned int>& point, const uint32_t width, const std::vector<int>& tiles);
      static bool m_is_coast_point(const Point<unsigned int>& point, const uint32_t width, const uint32_t height, const std::vector<int>& tiles);
      static bool m_is_water(const Point<unsigned int>& point, const int width, const uint32_t height, const std::vector<int>& tiles);
      static bool m_contains_island_area(const Point<unsigned int>& point, const float segment_width_h, const float segment_width_v, const uint32_t width, const uint32_t height, const std::vector<int>& island_mask);
      /* static void m_generate_main_river(std::vector<int>& tiles, const uint32_t width, const uint32_t height, const std::vector<GeometryPoint<unsigned int>>& geometry); */
      /* static bool m_boders_water(const GeometryPoint<unsigned int>& geometry_point); */
      static void m_flood_fill(const int value, const uint32_t x, const uint32_t y, std::vector<int>& tiles, const uint32_t width, const uint32_t height);
      static std::vector<Point<unsigned int>> m_get_bays(std::vector<Point<unsigned int>>& coastline, const std::vector<Point<unsigned int>>& island, const uint32_t width, const uint32_t height);
      static bool m_has_land_intersection(const Point<unsigned int>& point_a, const Point<unsigned int>& point_b, const std::vector<int>& tiles, const uint32_t width);
      static BayData m_get_bay_data(const Point<unsigned int>& point_a, const Point<unsigned int>& point_b, const std::vector<int>& tiles, const uint32_t width, const uint32_t height, const int minimum, std::vector<int>& mask);
  };
}
