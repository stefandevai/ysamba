#include "./terrain_generator.hpp"

#include <iostream>
#include <random>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace dl
{
  FastNoiseLite TerrainGenerator::m_noise = FastNoiseLite{1337};

  float TerrainGenerator::get_ellipse_gradient_value(const uint32_t x, const uint32_t y, const uint32_t width, const uint32_t height)
  {
    return (std::sin((x/static_cast<float>(width)) * M_PI)) * (std::sin((y / static_cast<float>(height)) * M_PI));
  }

  float TerrainGenerator::get_rectangle_gradient_value(const uint32_t x, const uint32_t y, const uint32_t width, const uint32_t height)
  {
    return sqrtf(pow((x / static_cast<float>(width - 1)) - 0.5f, 2.0f) + pow((y / static_cast<float>(height - 1)) - 0.5f, 2.0f));
  }

  std::vector<int> TerrainGenerator::generate(const uint32_t width, const uint32_t height)
  {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(1492,1920384);

    m_noise.SetSeed(dist(rng));
    m_noise.SetFrequency(0.015f);
    m_noise.SetFractalType(FastNoiseLite::FractalType::FractalType_FBm);
    m_noise.SetNoiseType(FastNoiseLite::NoiseType::NoiseType_OpenSimplex2);
    m_noise.SetFractalWeightedStrength(0.1);

    std::vector<int> tiles(width * height);

    for (uint32_t j = 0; j < height; ++j)
    {
      for (uint32_t i = 0; i < width; ++i)
      {
        const float inverted_gradient = 1.0f - get_ellipse_gradient_value(i, j, width, height); 
        const float noise_value =  m_noise.GetNoise(static_cast<float>(i), static_cast<float>(j)) - inverted_gradient * 0.6f;

        int tile_value;

        if (noise_value > 0.5f)
        {
          tile_value = 2;
        }
        else if (noise_value > 0.1f)
        {
          tile_value = 3;
        }
        else
        {
          tile_value = 1;
        }

        tiles[j*width + i] = tile_value;
      }
    }

    return tiles;
  }
}

