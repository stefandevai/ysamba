#pragma once

#include <cstdint>
#include <utility>
#include "../point.hpp"

namespace dl
{
  using Edge = std::pair<Point<std::uint32_t>, Point<std::uint32_t>>;

  struct Site
  {
    Point<std::uint32_t> center;
    std::vector<Edge> edges;
    bool is_coast = false;
  };

  struct IslandData
  {
    struct Structure
    {
      std::vector<Site> sites;
      std::vector<std::uint32_t> coast_indexes;
      std::vector<std::uint32_t> land_indexes;
    };

    std::vector<Point<std::uint32_t>> points;
    std::vector<int> mask;
    Structure structure;
    Point<std::uint32_t> top_left;
    Point<std::uint32_t> bottom_right;
  };
}
