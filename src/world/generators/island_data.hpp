#pragma once

#include <cstdint>
#include <utility>

#include "./lib/gal/diagram.hpp"
#include "core/maths/vector.hpp"
#include "world/point.hpp"

namespace dl
{
using Edge = std::pair<Point<int>, Point<int>>;
using Diagram = gal::Diagram<double>;
using Site = gal::Diagram<double>::Site;
using GALPoint = gal::Vector2<double>;

struct IslandData
{
  struct Structure
  {
    Diagram diagram = Diagram({});
    std::vector<const Site*> coast_sites;
    std::vector<const Site*> land_sites;
  };

  std::vector<Point<int>> points;
  std::vector<int> mask;
  Structure structure;
  Point<int> top_left;
  Point<int> bottom_right;
};

struct IslandNoiseParams
{
  // General
  float frequency = 0.005f;
  float tier_land = 0.002f;
  float display_mask = true;

  // First layer
  int layer_1_octaves = 4;
  float layer_1_lacunarity = 3.2f;
  float layer_1_gain = 0.32f;
  float layer_1_weighted_strength = 0.25f;
  float layer_1_terrace_multiplier = 0.34f;
  Vector2 layer_1_remap_from{-1.0, 1.0};
  Vector2 layer_1_remap_to{0.0f, 8.08};

  // Second layer
  int layer_2_seed_offset = 7;
  int layer_2_octave_count = 4;
  float layer_2_lacunarity = 1.42f;
  float layer_2_gain = 1.1f;
  float layer_2_weighted_strength = 0.42f;
  float layer_2_fade = 0.62f;
  float layer_2_smooth_rhs = -0.68f;
  float layer_2_smoothness = 1.76f;
};
}  // namespace dl
