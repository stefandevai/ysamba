#pragma once

#include <FastNoise/FastNoise.h>

namespace dl
{
struct IslandNoiseParams;
}

namespace dl::utils
{
const FastNoise::SmartNode<> get_island_noise_generator(const IslandNoiseParams& params);
void generate_silhouette_map(
    float* data, int x, int y, int width, int height, const IslandNoiseParams& params, int seed);
void generate_mountain_map(float* data, int x, int y, int width, int height, const IslandNoiseParams& params, int seed);
void generate_control_map(float* data, int x, int y, int width, int height, const IslandNoiseParams& params, int seed);
void generate_humidity_map(float* data, int width, int height, const IslandNoiseParams& params, int seed);
void generate_temperature_map(float* data, int width, int height, const IslandNoiseParams& params, int seed);
constexpr int array_index(int x, int y, int width)
{
  return y * width + x;
}
}  // namespace dl::utils
