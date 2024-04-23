#pragma once

#include <FastNoise/FastNoise.h>

namespace dl
{
struct IslandNoiseParams;
}

namespace dl::utils
{
void generate_silhouette_map(
    float* data, int x, int y, int width, int height, const IslandNoiseParams& params, int seed);
void generate_mountain_map(float* data, int x, int y, int width, int height, const IslandNoiseParams& params, int seed);
void generate_control_map(float* data, int x, int y, int width, int height, const IslandNoiseParams& params, int seed);
void generate_humidity_map(float* data, int width, int height, const IslandNoiseParams& params, int seed);
void generate_temperature_map(float* data, int width, int height, const IslandNoiseParams& params, int seed);
}  // namespace dl::utils
