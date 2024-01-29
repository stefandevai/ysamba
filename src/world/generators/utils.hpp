#pragma once

#include <FastNoise/FastNoise.h>

namespace dl
{
struct IslandNoiseParams;
}

namespace dl::utils
{
const FastNoise::SmartNode<FastNoise::Remap> get_island_noise_generator(const IslandNoiseParams& params);
}
