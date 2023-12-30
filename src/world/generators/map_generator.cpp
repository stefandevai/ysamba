#include "./map_generator.hpp"

#include <spdlog/spdlog.h>

#include <algorithm>
#include <chrono>
#include <cmath>

#include "./lib/fast_noise_lite.hpp"
#include "core/random.hpp"

double smoothstep(double edge0, double edge1, double x)
{
  x = std::clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
  return x * x * (3 - 2 * x);
}

namespace dl
{
void MapGenerator::generate(const int seed, const Vector3i& offset)
{
  // TEMP
  /* m_json.load("./data/world/map_generators/terrain.json"); */
  // TEMP

  /* spdlog::info("============================="); */
  /* spdlog::info("= STARTING WORLD GENERATION ="); */
  /* spdlog::info("=============================\n"); */
  /* spdlog::info("SEED: {}", seed); */
  /* spdlog::info("WIDTH: {}", width); */
  /* spdlog::info("HEIGHT: {}\n", height); */

  std::vector<double> raw_height_map(width * height);
  tiles = std::vector<Cell>(width * height * depth);
  height_map = std::vector<int>(width * height);

  /* auto start = std::chrono::high_resolution_clock::now(); */

  /* spdlog::info("Generating silhouette..."); */

  m_get_height_map(raw_height_map, seed, offset);

  for (int j = 0; j < height; ++j)
  {
    for (int i = 0; i < width; ++i)
    {
      const auto map_value = raw_height_map[j * width + i];
      const int k = static_cast<int>(map_value * (depth - 1));

      height_map[j * width + i] = k;

      if (k < 2)
      {
        tiles[k * width * height + j * width + i].id = 1;
      }
      else
      {
        tiles[k * width * height + j * width + i].id = 2;
      }

      /* if (k == 0) */
      /* { */
      /*   tiles[k * width * height + j * width + i].id = 1; */
      /* } */
      /* else if (k == 1) */
      /* { */
      /*   tiles[k * width * height + j * width + i].id = 2; */
      /* } */
      /* else if (k == 2) */
      /* { */
      /*   tiles[k * width * height + j * width + i].id = 3; */
      /* } */
      /* else if (k == 3) */
      /* { */
      /*   tiles[k * width * height + j * width + i].id = 6; */
      /* } */
      /* else if (k == 4) */
      /* { */
      /*   tiles[k * width * height + j * width + i].id = 12; */
      /* } */
      /* else if (k == 5) */
      /* { */
      /*   tiles[k * width * height + j * width + i].id = 13; */
      /* } */
      /* else if (k == 6) */
      /* { */
      /*   tiles[k * width * height + j * width + i].id = 14; */
      /* } */
      /* else if (k == 7) */
      /* { */
      /*   tiles[k * width * height + j * width + i].id = 23; */
      /* } */
      /* else if (k == 8) */
      /* { */
      /*   tiles[k * width * height + j * width + i].id = 15; */
      /* } */
      /* else if (k == 9) */
      /* { */
      /*   tiles[k * width * height + j * width + i].id = 18; */
      /* } */
      /* else */
      /* { */
      /*   tiles[k * width * height + j * width + i].id = 19; */
      /* } */

      /* else */
      /* { */
      /*   tiles[k * width * height + j * width + i] = 2; */
      /* } */

      /* if (k > 0) */
      /* { */
      /*   tiles[(k - 1) * width * height + j * width + i] = 15; */
      /* } */

      for (int z = 0; z < k; ++z)
      {
        tiles[z * width * height + j * width + i].id = 5;
      }
    }
  }

  /* auto stop = std::chrono::high_resolution_clock::now(); */
  /* auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start); */

  /* spdlog::info("World generation finished! It took {} milliseconds", duration.count()); */
}

void MapGenerator::set_size(const Vector3i& size)
{
  width = size.x;
  height = size.y;
  depth = size.z;
}

void MapGenerator::m_get_height_map(std::vector<double>& height_values, const int seed, const Vector3i& offset)
{
  const auto simplex_freq = m_json.object["simplex_freq"].get<float>();
  const auto simplex_octaves = m_json.object["simplex_octaves"].get<int>();
  const auto simplex_lacunarity = m_json.object["simplex_lacunarity"].get<float>();
  const auto simplex_gain = m_json.object["simplex_gain"].get<float>();
  const auto simplex_weighted_strength = m_json.object["simplex_weighted_strength"].get<float>();

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

  for (int j = 0; j < height; ++j)
  {
    for (int i = 0; i < width; ++i)
    {
      /* const float gradient = m_get_rectangle_gradient_value(i, j); */
      const float noise_value = noise.GetNoise(static_cast<float>(offset.x + i), static_cast<float>(offset.y + j));

      height_values[j * width + i] = noise_value;
      max_value = std::max(max_value, noise_value);
      min_value = std::min(min_value, noise_value);
    }
  }

  /* const double distance = max_value - min_value; */
  /* const double distance = 0.9; */

  for (size_t i = 0; i < height_values.size(); ++i)
  {
    height_values[i] = smoothstep(-1.3, 1.3, height_values[i]);
    /* /1* height_values[i] = (height_values[i] - min_value) / distance; *1/ */
    /* if (height_values[i] < 0.0) */
    /* { */
    /*   height_values[i] = 0.0; */
    /* } */
    /* if (height_values[i] > 1.0) */
    /* { */
    /*   height_values[i] = 1.0; */
    /* } */
  }
}

float MapGenerator::m_get_rectangle_gradient_value(const int x, const int y)
{
  auto distance_to_edge = std::min(abs(x - width), x);
  distance_to_edge = std::min(distance_to_edge, abs(y - height));
  distance_to_edge = std::min(distance_to_edge, y) * 2;
  return 1.f - static_cast<float>(distance_to_edge) / (width / 2.0f);
}

}  // namespace dl
