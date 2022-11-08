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

  float TerrainGenerator::get_rectangle_gradient_value(const int x, const int y, const int width, const int height)
  {
    auto distance_to_edge = std::min(abs(x - width), x);
    distance_to_edge = std::min(distance_to_edge, abs(y - height));
    distance_to_edge = std::min(distance_to_edge, y) * 2;
    return 1.f - static_cast<float>(distance_to_edge) / (width / 2.0f);
  }

  std::vector<int> TerrainGenerator::generate(const uint32_t width, const uint32_t height)
  {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(1492,1920384);

    m_noise.SetSeed(dist(rng));
    /* m_noise.SetSeed(110); */
    m_noise.SetNoiseType(FastNoiseLite::NoiseType::NoiseType_OpenSimplex2S);
    m_noise.SetRotationType3D(FastNoiseLite::RotationType3D_ImproveXYPlanes);
    m_noise.SetFrequency(0.005f);
    m_noise.SetFractalType(FastNoiseLite::FractalType::FractalType_FBm);
    m_noise.SetFractalOctaves(5);
    m_noise.SetFractalLacunarity(3.4f);
    m_noise.SetFractalGain(0.36f);
    m_noise.SetFractalWeightedStrength(0.25);

    std::vector<int> tiles(width * height);

    for (uint32_t j = 0; j < height; ++j)
    {
      for (uint32_t i = 0; i < width; ++i)
      {
        const float gradient = get_rectangle_gradient_value(i, j, width, height); 

        const float noise_value =  m_noise.GetNoise(static_cast<float>(i), static_cast<float>(j)) - gradient;

        int tile_value;

        if (noise_value > 0.11f)
        {
          tile_value = 2;
        }
        else if (noise_value > 0.002f)
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

