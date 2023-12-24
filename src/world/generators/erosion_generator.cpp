#include "./erosion_generator.hpp"

#include <spdlog/spdlog.h>

#include <chrono>
#include <cmath>

#include "./lib/fast_noise_lite.hpp"
#include "core/random.hpp"

namespace dl
{
struct Particle
{
  glm::vec2 pos;
  glm::vec2 speed = glm::vec2(0.0);
  float volume = 1.0;
  float sediment = 0.0;
};

void ErosionGenerator::generate(const int seed)
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
  m_generate_silhouette(raw_height_map, seed);

  tiles.clear();
  height_map.clear();
  /* const uint32_t cycles = 1000 * 10; */
  /* m_erode(height_map, cycles); */
  /* std::vector<int> tiles(m_width * m_height * m_z_levels); */

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

void ErosionGenerator::m_generate_silhouette(std::vector<double>& tiles, const int seed)
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

      tiles[j * m_width + i] = noise_value;
      max_value = std::max(max_value, noise_value);
      min_value = std::min(min_value, noise_value);
    }
  }

  spdlog::debug("MINMAX: {} {}", max_value, min_value);
  const double distance = max_value - min_value;

  // Normalize values between 0.0 and 1.0
  for (size_t i = 0; i < tiles.size(); ++i)
  {
    tiles[i] = (tiles[i] - min_value) / distance;
  }
}

float ErosionGenerator::m_get_rectangle_gradient_value(const int x, const int y)
{
  auto distance_to_edge = std::min(abs(x - m_width), x);
  distance_to_edge = std::min(distance_to_edge, abs(y - m_height));
  distance_to_edge = std::min(distance_to_edge, y) * 2;
  return 1.f - static_cast<float>(distance_to_edge) / (m_width / 2.0f);
}

void ErosionGenerator::m_erode(std::vector<double>& tiles, const uint32_t cycles)
{
  const float minVol = 0.01f;
  float dt = 1.2;
  float density = 1.0;  // This gives varying amounts of inertia and stuff...
  float evapRate = 0.001;
  float depositionRate = 0.1;
  float friction = 0.05;

  for (uint32_t i = 0; i < cycles; ++i)
  {
    glm::vec2 newpos = glm::vec2(rand() % m_width - 10 + 5, rand() % m_height - 10 + 5);
    Particle drop{newpos};

    int iteration = 0;

    while (drop.volume > minVol)
    {
      ++iteration;
      glm::ivec2 ipos = drop.pos;
      glm::vec3 n = m_surface_normal(tiles, ipos.x, ipos.y);

      drop.speed += dt * glm::vec2(n.x, n.z) / (drop.volume * density);  // F = ma, so a = F/m
      drop.pos += dt * drop.speed;
      drop.speed *= (1.0 - dt * friction);

      // Check if Particle is still in-bounds
      if (!glm::all(glm::greaterThanEqual(drop.pos, glm::vec2(5))) ||
          !glm::all(glm::lessThan(drop.pos, glm::vec2(m_width - 5, m_height - 5))))
        break;

      // Compute sediment capacity difference
      float maxsediment = drop.volume * glm::length(drop.speed) *
                          (tiles[ipos.x + ipos.y * m_width] - tiles[drop.pos.x + drop.pos.y * m_width]);
      if (maxsediment < 0.0)
        maxsediment = 0.0;
      float sdiff = maxsediment - drop.sediment;

      // Act on the Heightmap and Droplet!
      drop.sediment += dt * depositionRate * sdiff;
      tiles[ipos.x + ipos.y * m_width] -= dt * drop.volume * depositionRate * sdiff;

      // Evaporate the Droplet (Note: Proportional to Volume! Better: Use shape factor to make proportional to the area
      // instead.)
      drop.volume *= (1.0 - dt * evapRate);
    }

    /* spdlog::debug("IT: {}", iteration); */
  }
}

glm::vec3 ErosionGenerator::m_surface_normal(const std::vector<double>& height_map, int i, int j)
{
  if (i + 1 >= m_width || i - 1 < 0 || j + 1 >= m_height || j - 1 < 0)
  {
    return glm::vec3(0.0);
  }

  const float scale = 60.f;
  /*
    Note: Surface normal is computed in this way, because the square-grid surface is meshed using triangles.
    To avoid spatial artifacts, you need to weight properly with all neighbors.
  */

  glm::vec3 n = glm::vec3(0.15) *
                glm::normalize(glm::vec3(
                    scale * (height_map[i + j * m_width] - height_map[i + 1 + j * m_width]), 1.0, 0.0));  // Positive X

  n += glm::vec3(0.15) *
       glm::normalize(
           glm::vec3(scale * (height_map[i - 1 + j * m_width] - height_map[i + j * m_width]), 1.0, 0.0));  // Negative X

  n += glm::vec3(0.15) *
       glm::normalize(glm::vec3(
           0.0, 1.0, scale * (height_map[i + j * m_width] - height_map[i + (j + 1) * m_width])));  // Positive Y

  n += glm::vec3(0.15) *
       glm::normalize(glm::vec3(
           0.0, 1.0, scale * (height_map[i + (j - 1) * m_width] - height_map[i + j * m_width])));  // Negative Y

  // Diagonals! (This removes the last spatial artifacts)
  n += glm::vec3(0.1) *
       glm::normalize(glm::vec3(
           scale * (height_map[i + j * m_width] - height_map[i + 1 + (j + 1) * m_width]) / sqrt(2),
           sqrt(2),
           scale * (height_map[i + j * m_width] - height_map[i + 1 + (j + 1) * m_width]) / sqrt(2)));  // Positive Y

  n += glm::vec3(0.1) *
       glm::normalize(glm::vec3(
           scale * (height_map[i + j * m_width] - height_map[i + 1 + (j - 1) * m_width]) / sqrt(2),
           sqrt(2),
           scale * (height_map[i + j * m_width] - height_map[i + 1 + (j - 1) * m_width]) / sqrt(2)));  // Positive Y

  n += glm::vec3(0.1) *
       glm::normalize(glm::vec3(
           scale * (height_map[i + j * m_width] - height_map[i - 1 + (j + 1) * m_width]) / sqrt(2),
           sqrt(2),
           scale * (height_map[i + j * m_width] - height_map[i - 1 + (j + 1) * m_width]) / sqrt(2)));  // Positive Y

  n += glm::vec3(0.1) *
       glm::normalize(glm::vec3(
           scale * (height_map[i + j * m_width] - height_map[i - 1 + (j - 1) * m_width]) / sqrt(2),
           sqrt(2),
           scale * (height_map[i + j * m_width] - height_map[i - 1 + (j - 1) * m_width]) / sqrt(2)));  // Positive Y

  return n;
}
}  // namespace dl
