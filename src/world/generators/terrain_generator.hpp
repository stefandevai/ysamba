#pragma once

#include <vector>
#include <queue>
#include "../../core/lua_api.hpp"
#include "../point.hpp"
#include "./lib/fast_noise_lite.hpp"
#include "./island_data.hpp"

namespace dl
{
  class CompareVectorSizes
  {
    public:
      bool operator() (const IslandData& a, const IslandData& b) const
      {
        return a.points.size() > b.points.size();
      }
  };

  struct BayData
  {
    BayData(const int area, const Point<int>& point) : area(area), point(point) {}
    int area = 0;
    Point<int> point = Point<int>(0, 0);
  };

  struct RiverSegment
  {
    Point<int> point;
    Point<double> center;
    Point<double> normal;
    double length = 0.0;
    std::shared_ptr<RiverSegment> previous;
    std::shared_ptr<RiverSegment> next;
  };

  using IslandQueue = std::priority_queue<IslandData, std::vector<IslandData>, CompareVectorSizes>;

  class TerrainGenerator
  {
    public:
      static std::vector<int> generate(const int width, const int height, const int seed);

    private:
      static inline FastNoiseLite m_noise = FastNoiseLite{1337};
      static inline LuaAPI m_lua = LuaAPI{"generators/terrain.lua"};

      static void m_generate_silhouette(std::vector<int>& tiles, const int width, const int height, const int seed);
      static float m_get_rectangle_gradient_value(const int x, const int y, const int width, const int height);
      static std::vector<IslandData> m_get_islands(std::vector<int>& tiles, const int width, const int height);
      static IslandQueue m_get_island_queue(const std::vector<int>& tiles, const int width, const int heights);
      static IslandData m_get_island(const std::vector<int>& tiles, std::vector<int>& mask, const int x, const int y, const int width, const int height);
      static bool m_valid_coord(const int x, const int y, const int width, const int height);
      static bool m_valid_point(const Point<int>& point, const int width, const int height);
      static std::vector<Point<int>> m_get_coastline(const std::vector<int>& tiles, const int width, const int height, const IslandData& island);
      static Point<int> m_get_first_coastline_point(const std::vector<int>& tiles, const int width, const int height, const IslandData& island);
      static int m_get_point_value(const Point<int>& point, const int width, const std::vector<int>& tiles);
      static bool m_is_coast_point(const Point<int>& point, const int width, const int height, const std::vector<int>& tiles);
      static bool m_is_water(const Point<int>& point, const int width, const int height, const std::vector<int>& tiles);
      static bool m_contains_island_area(const Point<int>& point, const float segment_width_h, const float segment_width_v, const int width, const int height, const std::vector<int>& island_mask);
      static void m_flood_fill(const int value, const int x, const int y, std::vector<int>& tiles, const int width, const int height);
      static std::vector<Point<int>> m_get_bays(std::vector<Point<int>>& coastline, const IslandData& island, const int width, const int height);
      static bool m_has_land_intersection(const Point<int>& point_a, const Point<int>& point_b, const std::vector<int>& tiles, const int width, const int water_value);
      static BayData m_get_bay_data(const Point<int>& point_a, const Point<int>& point_b, const std::vector<int>& tiles, const int width, const int height, const int minimum, std::vector<int>& mask, const int water_value);
      static void m_build_island_structure(IslandData& island, const int width, const int height);
      static bool m_center_is_coast(const Point<int>& center, const std::vector<int>& island_mask, const int width, const int height);
      static void m_generate_main_river(IslandData& island, std::vector<Point<int>>& bays, std::vector<int>& tiles, const int width, const int height, const int seed = 0);
      static std::pair<Point<int>, Point<int>> m_get_river_source_and_mouth(IslandData& island, std::vector<Point<int>>& bays, const int width, const int height, const int seed);
      static std::vector<std::shared_ptr<RiverSegment>> m_get_river_segments(const Point<int>& source, const Point<int>& mouth, std::vector<int>& tiles, const int width, const int seed);
      static void m_create_meanders(std::vector<std::shared_ptr<RiverSegment>>& river, std::vector<int>& tiles, const int width, const int height);
      static float m_distance(const Point<int>& point_a, const Point<int>& point_b);
      static double m_distance(const Point<double>& point_a, const Point<double>& point_b);
      static double m_menger_curvature(const Point<int>& point_a, const Point<int>& point_b, const Point<int>& point_c, const double length_1, const double length_2, const double length_3);

      // Debug functions
      static void m_draw_point(const Point<int>& point, const int value, std::vector<int>& tiles, const int width);
      static void m_draw_big_point(const Point<int>& point, const int value, std::vector<int>& tiles, const int width);
      static void m_draw_line(const Point<int>& origin, const Point<int>& destination, const int value, std::vector<int>& tiles, const int width, const int height);
  };
}
