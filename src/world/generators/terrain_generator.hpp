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
    BayData(const int area, const Point<std::uint32_t>& point) : area(area), point(point) {}
    int area = 0;
    Point<std::uint32_t> point = Point<std::uint32_t>(0, 0);
  };

  struct RiverSegment
  {
    Point<std::uint32_t> point;
    std::shared_ptr<RiverSegment> next;
  };

  using IslandQueue = std::priority_queue<IslandData, std::vector<IslandData>, CompareVectorSizes>;

  class TerrainGenerator
  {
    public:
      static std::vector<int> generate(const uint32_t width, const uint32_t height, const int seed);

    private:
      static inline FastNoiseLite m_noise = FastNoiseLite{1337};
      static inline LuaAPI m_lua = LuaAPI{"generators/terrain.lua"};

      static void m_generate_silhouette(std::vector<int>& tiles, const uint32_t width, const uint32_t height, const int seed);
      static float m_get_rectangle_gradient_value(const int x, const int y, const int width, const int height);
      static std::vector<IslandData> m_get_islands(std::vector<int>& tiles, const int width, const int height);
      static IslandQueue m_get_island_queue(const std::vector<int>& tiles, const uint32_t width, const uint32_t heights);
      static IslandData m_get_island(const std::vector<int>& tiles, std::vector<int>& mask, const uint32_t x, const uint32_t y, const uint32_t width, const uint32_t height);
      static bool m_valid_coord(const uint32_t x, const uint32_t y, const uint32_t width, const uint32_t height);
      static bool m_valid_point(const Point<std::uint32_t>& point, const uint32_t width, const uint32_t height);
      static std::vector<Point<std::uint32_t>> m_get_coastline(const std::vector<int>& tiles, const uint32_t width, const uint32_t height, const IslandData& island);
      static Point<std::uint32_t> m_get_first_coastline_point(const std::vector<int>& tiles, const uint32_t width, const uint32_t height, const IslandData& island);
      static int m_get_point_value(const Point<std::uint32_t>& point, const uint32_t width, const std::vector<int>& tiles);
      static bool m_is_coast_point(const Point<std::uint32_t>& point, const uint32_t width, const uint32_t height, const std::vector<int>& tiles);
      static bool m_is_water(const Point<std::uint32_t>& point, const int width, const uint32_t height, const std::vector<int>& tiles);
      static bool m_contains_island_area(const Point<std::uint32_t>& point, const float segment_width_h, const float segment_width_v, const uint32_t width, const uint32_t height, const std::vector<int>& island_mask);
      static void m_flood_fill(const int value, const uint32_t x, const uint32_t y, std::vector<int>& tiles, const uint32_t width, const uint32_t height);
      static std::vector<Point<std::uint32_t>> m_get_bays(std::vector<Point<std::uint32_t>>& coastline, const IslandData& island, const uint32_t width, const uint32_t height);
      static bool m_has_land_intersection(const Point<std::uint32_t>& point_a, const Point<std::uint32_t>& point_b, const std::vector<int>& tiles, const uint32_t width, const int water_value);
      static BayData m_get_bay_data(const Point<std::uint32_t>& point_a, const Point<std::uint32_t>& point_b, const std::vector<int>& tiles, const uint32_t width, const uint32_t height, const int minimum, std::vector<int>& mask, const int water_value);
      static void m_build_island_structure(IslandData& island, const uint32_t width, const uint32_t height);
      static bool m_center_is_coast(const Point<std::uint32_t>& center, const std::vector<int>& island_mask, const uint32_t width, const uint32_t height);
      static void m_generate_main_river(IslandData& island, std::vector<Point<std::uint32_t>>& bays, std::vector<int>& tiles, const std::uint32_t width, const std::uint32_t height, const int seed = 0);
      static std::pair<Point<std::uint32_t>, Point<std::uint32_t>> m_get_river_source_and_mouth(IslandData& island, std::vector<Point<std::uint32_t>>& bays, const std::uint32_t height, const int seed);
      static std::vector<std::shared_ptr<RiverSegment>> m_get_river_segments(const Point<std::uint32_t>& source, const Point<std::uint32_t>& mouth, const std::vector<int>& tiles, const std::uint32_t width, const int seed);
      static float m_distance(const Point<std::uint32_t>& point_a, const Point<std::uint32_t>& point_b);

      // Debug functions
      static void m_draw_point(const Point<std::uint32_t>& point, const int value, std::vector<int>& tiles, const std::uint32_t width);
      static void m_draw_big_point(const Point<std::uint32_t>& point, const int value, std::vector<int>& tiles, const std::uint32_t width);
      static void m_draw_line(const Point<std::uint32_t>& origin, const Point<std::uint32_t>& destination, const int value, std::vector<int>& tiles, const uint32_t width, const uint32_t height);
  };
}
