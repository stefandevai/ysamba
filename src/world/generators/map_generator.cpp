#include "./map_generator.hpp"

#include <spdlog/spdlog.h>

#include <chrono>
#include <cmath>

#include "./lib/fast_noise_lite.hpp"
#include "core/random.hpp"

namespace dl
{
void MapGenerator::generate(const int seed)
{
  // TEMP
  m_lua.load("generators/terrain.lua");
  // TEMP

  spdlog::info("=============================");
  spdlog::info("= STARTING WORLD GENERATION =");
  spdlog::info("=============================\n");
  spdlog::info("SEED: {}", seed);
  spdlog::info("WIDTH: {}", m_width);
  spdlog::info("HEIGHT: {}\n", m_height);

  std::vector<double> raw_height_map(m_width * m_height);

  auto start = std::chrono::high_resolution_clock::now();

  spdlog::info("Generating silhouette...");
  m_get_height_map(raw_height_map, seed);

  tiles.clear();
  height_map.clear();

  for (int j = 0; j < m_height; ++j)
  {
    for (int i = 0; i < m_width; ++i)
    {
      const auto map_value = raw_height_map[j * m_width + i];
      const int k = static_cast<int>(map_value * (m_z_levels - 1));

      height_map[j * m_width + i] = k;

      /* if (k < 2) */
      /* { */
      /*   tiles[k * m_width * m_height + j * m_width + i] = 1; */
      /* } */
      /* else */
      /* { */
      /*   tiles[k * m_width * m_height + j * m_width + i] = 2; */
      /* } */

      if (k == 0)
      {
        tiles[k * m_width * m_height + j * m_width + i] = 1;
      }
      else if (k == 1)
      {
        tiles[k * m_width * m_height + j * m_width + i] = 2;
      }
      else if (k == 2)
      {
        tiles[k * m_width * m_height + j * m_width + i] = 3;
      }
      else if (k == 3)
      {
        tiles[k * m_width * m_height + j * m_width + i] = 6;
      }
      else if (k == 4)
      {
        tiles[k * m_width * m_height + j * m_width + i] = 12;
      }
      else if (k == 5)
      {
        tiles[k * m_width * m_height + j * m_width + i] = 13;
      }
      else if (k == 6)
      {
        tiles[k * m_width * m_height + j * m_width + i] = 14;
      }
      else if (k == 7)
      {
        tiles[k * m_width * m_height + j * m_width + i] = 23;
      }
      else if (k == 8)
      {
        tiles[k * m_width * m_height + j * m_width + i] = 15;
      }
      else if (k == 9)
      {
        tiles[k * m_width * m_height + j * m_width + i] = 18;
      }
      else
      {
        tiles[k * m_width * m_height + j * m_width + i] = 19;
      }

      /* else */
      /* { */
      /*   tiles[k * m_width * m_height + j * m_width + i] = 2; */
      /* } */

      /* if (k > 0) */
      /* { */
      /*   tiles[(k - 1) * m_width * m_height + j * m_width + i] = 15; */
      /* } */

      /* for (int z = 0; z < k; ++z) */
      /* { */
      /*   tiles[z * m_width * m_height + j * m_width + i] = 5; */
      /* } */
    }
  }

  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

  spdlog::info("World generation finished! It took {} milliseconds", duration.count());
}

void MapGenerator::m_get_height_map(std::vector<double>& height_values, const int seed)
{
  const auto simplex_freq = m_lua.get_variable<float>("simplex_freq");
  const auto simplex_octaves = m_lua.get_variable<int>("simplex_octaves");
  const auto simplex_lacunarity = m_lua.get_variable<float>("simplex_lacunarity");
  const auto simplex_gain = m_lua.get_variable<float>("simplex_gain");
  const auto simplex_weighted_strength = m_lua.get_variable<float>("simplex_weighted_strength");

  auto noise = FastNoiseLite{seed};
  noise.SetSeed(seed);
  noise.SetNoiseType(FastNoiseLite::NoiseType::NoiseType_OpenSimplex2S);
  noise.SetRotationType3D(FastNoiseLite::RotationType3D_ImproveXYPlanes);
  noise.SetFrequency(simplex_freq);
  noise.SetFractalType(FastNoiseLite::FractalType::FractalType_FBm);
  noise.SetFractalOctaves(simplex_octaves);
  noise.SetFractalLacunarity(simplex_lacunarity);
  noise.SetFractalGain(simplex_gain);
  noise.SetFractalWeightedStrength(simplex_weighted_strength);

  float max_value = 0.0;
  float min_value = 1.0;

  for (int j = 0; j < m_height; ++j)
  {
    for (int i = 0; i < m_width; ++i)
    {
      const float gradient = m_get_rectangle_gradient_value(i, j);
      const float noise_value = noise.GetNoise(static_cast<float>(i), static_cast<float>(j)) - gradient;

      height_values[j * m_width + i] = noise_value;
      max_value = std::max(max_value, noise_value);
      min_value = std::min(min_value, noise_value);
    }
  }

  const double distance = max_value - min_value;

  // Normalize values between 0.0 and 1.0
  for (size_t i = 0; i < height_values.size(); ++i)
  {
    height_values[i] = (height_values[i] - min_value) / distance;
  }
}

float MapGenerator::m_get_rectangle_gradient_value(const int x, const int y)
{
  auto distance_to_edge = std::min(abs(x - m_width), x);
  distance_to_edge = std::min(distance_to_edge, abs(y - m_height));
  distance_to_edge = std::min(distance_to_edge, y) * 2;
  return 1.f - static_cast<float>(distance_to_edge) / (m_width / 2.0f);
}

}  // namespace dl
