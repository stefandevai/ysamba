#pragma once

#include <FastNoise/FastNoise.h>

namespace dl
{
struct IslandNoiseParams;
}

namespace dl::utils
{
const FastNoise::SmartNode<> get_island_noise_generator(const IslandNoiseParams& params);
void generate_silhouette_map(float* data, int x, int y, int width, int height, const IslandNoiseParams& params, int seed);
void generate_mountain_map(float* data, int x, int y, int width, int height, const IslandNoiseParams& params, int seed);
}
