#include "./terrain_generator.hpp"

#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace dl
{
  FastNoiseLite TerrainGenerator::m_noise = FastNoiseLite{1337};
  LuaAPI TerrainGenerator::m_lua = LuaAPI{"generators/terrain.lua"};

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

  std::vector<int> TerrainGenerator::generate(const uint32_t width, const uint32_t height, const int seed)
  {
    m_lua.load("generators/terrain.lua");

    // Generate whole continent
    const auto simplex_freq = m_lua.get_variable<float>("simplex_freq");
    const auto simplex_octaves = m_lua.get_variable<int>("simplex_octaves");
    const auto simplex_lacunarity = m_lua.get_variable<float>("simplex_lacunarity");
    const auto simplex_gain = m_lua.get_variable<float>("simplex_gain");
    const auto simplex_weighted_strength = m_lua.get_variable<float>("simplex_weighted_strength");

    m_noise.SetSeed(seed);
    m_noise.SetNoiseType(FastNoiseLite::NoiseType::NoiseType_OpenSimplex2S);
    m_noise.SetRotationType3D(FastNoiseLite::RotationType3D_ImproveXYPlanes);
    m_noise.SetFrequency(simplex_freq);
    m_noise.SetFractalType(FastNoiseLite::FractalType::FractalType_FBm);
    m_noise.SetFractalOctaves(simplex_octaves);
    m_noise.SetFractalLacunarity(simplex_lacunarity);
    m_noise.SetFractalGain(simplex_gain);
    m_noise.SetFractalWeightedStrength(simplex_weighted_strength);

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

    // Generate relief
    const auto cellular_freq = m_lua.get_variable<float>("cellular_freq");
    const auto cellular_octaves = m_lua.get_variable<int>("cellular_octaves");
    const auto cellular_lacunarity = m_lua.get_variable<float>("cellular_lacunarity");
    const auto cellular_gain = m_lua.get_variable<float>("cellular_gain");
    const auto cellular_weighted_strength = m_lua.get_variable<float>("cellular_weighted_strength");

    m_noise.SetNoiseType(FastNoiseLite::NoiseType::NoiseType_Cellular);
    m_noise.SetRotationType3D(FastNoiseLite::RotationType3D_ImproveXYPlanes);
    m_noise.SetFrequency(cellular_freq);
    m_noise.SetFractalType(FastNoiseLite::FractalType::FractalType_Ridged);
    m_noise.SetFractalOctaves(cellular_octaves);
    m_noise.SetFractalLacunarity(cellular_lacunarity);
    m_noise.SetFractalGain(cellular_gain);
    m_noise.SetFractalWeightedStrength(cellular_weighted_strength);
    m_noise.SetCellularDistanceFunction(FastNoiseLite::CellularDistanceFunction_EuclideanSq);
    m_noise.SetCellularReturnType(FastNoiseLite::CellularReturnType_Distance2Add);

    for (uint32_t j = 0; j < height; ++j)
    {
      for (uint32_t i = 0; i < width; ++i)
      {
        const float gradient = get_rectangle_gradient_value(i, j, width, height); 
        const float noise_value =  m_noise.GetNoise(static_cast<float>(i), static_cast<float>(j)) * gradient;
        const int original_tile_value = tiles[j*width + i];

        // Water tile
        if (original_tile_value == 1)
        {
          continue;
        }

        auto tile_value = original_tile_value;

        /* if (noise_value > 0.4f) */
        /* { */
        /*   tile_value = 4; */
        /* } */
        /* else if (noise_value > -0.3f) */
        /* { */
        /*   tile_value = 5; */
        /* } */
        /* else if (noise_value > -0.5f) */
        /* { */
        /*   tile_value = 6; */
        /* } */

        if (noise_value < -0.5f)
        {
          tile_value = 1;
        }
        /* else if (noise_value < -0.4f) */
        /* { */
        /*   tile_value = 5; */
        /* } */
        /* else if (noise_value < -0.28f) */
        /* { */
        /*   tile_value = 6; */
        /* } */

        tiles[j*width + i] = tile_value;
      }
    }

    return tiles;
  }
}

