#pragma once

#include <cstdint>
#include <utility>
#include "../point.hpp"

namespace dl
{
  using Edge = std::pair<Point<int>, Point<int>>;

  struct Site
  {
    Point<int> center;
    std::vector<Edge> edges;
    bool is_coast = false;
  };

  struct IslandData
  {
    struct Structure
    {
      std::vector<Site> sites;
      std::vector<int> coast_indexes;
      std::vector<int> land_indexes;
    };

    std::vector<Point<int>> points;
    std::vector<int> mask;
    Structure structure;
    Point<int> top_left;
    Point<int> bottom_right;
  };
}
