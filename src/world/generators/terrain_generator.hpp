#pragma once

#include <vector>
#include <queue>
#include "../../core/lua_api.hpp"
#include "../point.hpp"
#include "./fast_noise_lite.hpp"


namespace dl
{
  class CompareVectorSizes
  {
    public:
      bool operator() (const std::vector<Point>& a, const std::vector<Point>& b) const
      {
        return a.size() > b.size();
      }
  };

  typedef std::priority_queue<std::vector<Point>, std::vector<std::vector<Point>>, CompareVectorSizes> island_queue;

  class TerrainGenerator
  {
    public:
      static std::vector<int> generate(const uint32_t width, const uint32_t height, const int seed);

    private:
      static LuaAPI m_lua;
      static FastNoiseLite m_noise;

      static void m_generate_silhouette(std::vector<int>& tiles, const uint32_t width, const uint32_t height, const int seed);
      static float m_get_rectangle_gradient_value(const int x, const int y, const int width, const int height);
      static std::vector<std::vector<Point>> m_adjust_and_get_islands(std::vector<int>& tiles, const int width, const int height);
      static island_queue m_get_islands(const std::vector<int>& tile, const uint32_t width, const uint32_t heights);
      static std::vector<Point> m_get_island(const std::vector<int>& tiles, std::vector<int>& mask, const uint32_t x, const uint32_t y, const uint32_t width, const uint32_t height);
      static bool m_valid_coord(const uint32_t x, const uint32_t y, const uint32_t width, const uint32_t height);
      static bool m_valid_point(const Point& point, const uint32_t width, const uint32_t height);
      static void m_generate_main_river(std::vector<int>& tiles, const uint32_t width, const uint32_t height, const std::vector<Point>& main_island);
      static std::vector<Point> m_get_coastline(std::vector<int>& tiles, const uint32_t width, const uint32_t height, const std::vector<Point>& main_island);
      static Point m_get_first_coastline_point(const std::vector<int>& tiles, const uint32_t width, const uint32_t height, const std::vector<Point>& main_island);
      static int m_get_point_value(const Point& point, const uint32_t width, const std::vector<int>& tiles);
      static bool m_is_coast_point(const Point& point, const uint32_t width, const uint32_t height, const std::vector<int>& tiles);
      static bool m_is_water(const Point& point, const int width, const uint32_t height, const std::vector<int>& tiles);
  };
}
