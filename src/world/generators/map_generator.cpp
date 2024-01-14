#include "./map_generator.hpp"

#include <FastNoise/FastNoise.h>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <chrono>
#include <cmath>

#include "core/random.hpp"
#include "world/chunk.hpp"

float smoothstep(float edge0, float edge1, float x)
{
  x = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
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

  const int padded_width = width + m_generation_padding * 2;
  const int padded_height = height + m_generation_padding * 2;

  chunk = std::make_unique<Chunk>(Vector3i{0, 0, 0}, true);
  chunk->position = offset;
  chunk->tiles.set_size(width, height, depth);
  // tiles = std::vector<Cell>(width * height * depth);
  // height_map = std::vector<int>(width * height);

  raw_height_map.resize(padded_width * padded_height);
  auto terrain = std::vector<int>(padded_width * padded_height * depth);

  /* auto start = std::chrono::high_resolution_clock::now(); */

  /* spdlog::info("Generating silhouette..."); */

  m_get_height_map(raw_height_map, seed, offset);

  for (int j = 0; j < padded_height; ++j)
  {
    for (int i = 0; i < padded_width; ++i)
    {
      const auto map_value = raw_height_map[j * padded_width + i];
      const int k = static_cast<int>(map_value * (depth - 1));
      bool inside_chunk = false;

      if (j >= m_generation_padding && j < padded_width - m_generation_padding && i >= m_generation_padding &&
          i < padded_height - m_generation_padding)
      {
        inside_chunk = true;
      }

      int terrain_id = 0;

      if (k < 2)
      {
        terrain_id = 1;
      }
      else
      {
        terrain_id = 2;
      }

      if (inside_chunk)
      {
        chunk->tiles.height_map[(j - 1) * width + (i - 1)] = k;
        chunk->tiles.values[k * width * height + (j - 1) * width + (i - 1)].terrain = terrain_id;
      }

      terrain[k * padded_width * padded_height + j * padded_width + i] = terrain_id;

      for (int z = 0; z < k; ++z)
      {
        terrain[z * padded_width * padded_height + j * padded_width + i] = terrain_id;

        if (inside_chunk)
        {
          chunk->tiles.values[z * width * height + (j - 1) * width + (i - 1)].terrain = terrain_id;
        }
      }
    }
  }

  chunk->tiles.compute_visibility();

  /* auto stop = std::chrono::high_resolution_clock::now(); */
  /* auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start); */

  /* spdlog::info("World generation finished! It took {} milliseconds", duration.count()); */

  for (int k = 0; k < depth; ++k)
  {
    for (int j = 0; j < height; ++j)
    {
      for (int i = 0; i < width; ++i)
      {
        m_select_tile(terrain, i, j, k);
      }
    }
  }
}

void MapGenerator::set_size(const Vector3i& size)
{
  width = size.x;
  height = size.y;
  depth = size.z;
}

void MapGenerator::m_get_height_map(std::vector<float>& height_values, const int seed, const Vector3i& offset)
{
  /* const float frequency = 0.005f; */
  const float frequency = 0.005f;
  // (2D) (((OpenSimplex2S + FractalRidged(G0.5 W0.0, O3, L2)) + (OpenSimplex2S + SeedOffset(S10) + FractalFBm(G0.5,
  // W0.0, O5, L2.0))) + MinSmooth(S2.46))
  FastNoise::SmartNode<> noise_generator = FastNoise::NewFromEncodedNodeTree(
      "HwAPAAMAAAAAAABAKQAAAAAAPwAAAAAAAQ0ABQAAAAAAAEAWAAoAAAApAAAAAAA/AAAAAAAApHAdQA==");
  noise_generator->GenUniformGrid2D(height_values.data(),
                                    offset.x - m_generation_padding,
                                    offset.y + m_generation_padding,
                                    width + m_generation_padding * 2,
                                    height + m_generation_padding * 2,
                                    frequency,
                                    seed);

  /* const auto simplex_freq = m_json.object["simplex_freq"].get<float>(); */
  /* const auto simplex_octaves = m_json.object["simplex_octaves"].get<int>(); */
  /* const auto simplex_lacunarity = m_json.object["simplex_lacunarity"].get<float>(); */
  /* const auto simplex_gain = m_json.object["simplex_gain"].get<float>(); */
  /* const auto simplex_weighted_strength = m_json.object["simplex_weighted_strength"].get<float>(); */

  /* auto noise = FastNoiseLite{seed}; */
  /* noise.SetSeed(seed); */
  /* noise.SetNoiseType(FastNoiseLite::NoiseType::NoiseType_OpenSimplex2S); */
  /* noise.SetRotationType3D(FastNoiseLite::RotationType3D_ImproveXYPlanes); */
  /* noise.SetFrequency(simplex_freq); */
  /* noise.SetFractalType(FastNoiseLite::FractalType::FractalType_FBm); */
  /* noise.SetFractalOctaves(simplex_octaves); */
  /* noise.SetFractalLacunarity(simplex_lacunarity); */
  /* noise.SetFractalGain(simplex_gain); */
  /* noise.SetFractalWeightedStrength(simplex_weighted_strength); */

  /* float max_value = 0.0; */
  /* float min_value = 1.0; */

  /* for (int j = 0; j < height; ++j) */
  /* { */
  /*   for (int i = 0; i < width; ++i) */
  /*   { */
  /*     /1* const float gradient = m_get_rectangle_gradient_value(i, j); *1/ */
  /*     /1* const float noise_value = noise.GetNoise(static_cast<float>(offset.x + i), static_cast<float>(offset.y +
   * j)); *1/ */

  /*     /1* height_values[j * width + i] = noise_value; *1/ */
  /*     max_value = std::max(max_value, height_values[j*width + i]); */
  /*     min_value = std::min(min_value, height_values[j*width + i]); */
  /*   } */
  /* } */

  /* spdlog::debug("MIN MAX {} {}", min_value, max_value); */

  /* const double distance = max_value - min_value; */
  /* const double distance = 0.9; */

  for (size_t i = 0; i < height_values.size(); ++i)
  {
    height_values[i] = smoothstep(-0.9, 0.3, height_values[i]);
    /* height_values[i] = (height_values[i] - min_value) / distance; */
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

void MapGenerator::m_select_tile(const std::vector<int>& terrain, const int x, const int y, const int z)
{
  const int padded_width = width + m_generation_padding * 2;
  const int padded_height = height + m_generation_padding * 2;
  const int transposed_x = x + m_generation_padding;
  const int transposed_y = y + m_generation_padding;

  const auto terrain_id = terrain[z * padded_width * padded_height + transposed_y * padded_width + transposed_x];

  if (terrain_id == 0)
  {
    return;
  }
  if (terrain_id == 1)
  {
    chunk->tiles.values[z * width * height + y * width + x].terrain = terrain_id;
    return;
  }

  // Set not walkable block tile if not visible
  if (!chunk->tiles.has_flags(DL_CELL_FLAG_VISIBLE, x, y, z))
  {
    chunk->tiles.values[z * width * height + y * width + x].terrain = 1;
    return;
  }

  const auto bitmask = m_get_bitmask(terrain, transposed_x, transposed_y, z);

  switch (bitmask)
  {
  case DL_EDGE_NONE:
  {
    const auto prob = random::get_real();
    if (prob < 0.1)
    {
      chunk->tiles.values[z * width * height + y * width + x].terrain = 5;
    }
    else
    {
      chunk->tiles.values[z * width * height + y * width + x].terrain = 38;
    }

    if (prob > 0.1 && prob < 0.11)
    {
      chunk->tiles.values[z * width * height + y * width + x].decoration = 10;
    }
    break;
  }
  case DL_EDGE_TOP:
    chunk->tiles.values[z * width * height + y * width + x].terrain = 37;
    break;
  case DL_EDGE_RIGHT:
    chunk->tiles.values[z * width * height + y * width + x].terrain = 34;
    break;
  case DL_EDGE_BOTTOM:
    chunk->tiles.values[z * width * height + y * width + x].terrain = 30;
    break;
  case DL_EDGE_LEFT:
    chunk->tiles.values[z * width * height + y * width + x].terrain = 36;
    break;
  case DL_EDGE_TOP | DL_EDGE_RIGHT:
    chunk->tiles.values[z * width * height + y * width + x].terrain = 33;
    break;
  case DL_EDGE_TOP | DL_EDGE_BOTTOM:
    chunk->tiles.values[z * width * height + y * width + x].terrain = 29;
    break;
  case DL_EDGE_TOP | DL_EDGE_LEFT:
    chunk->tiles.values[z * width * height + y * width + x].terrain = 35;
    break;
  case DL_EDGE_RIGHT | DL_EDGE_BOTTOM:
    chunk->tiles.values[z * width * height + y * width + x].terrain = 26;
    break;
  case DL_EDGE_RIGHT | DL_EDGE_LEFT:
    chunk->tiles.values[z * width * height + y * width + x].terrain = 32;
    break;
  case DL_EDGE_BOTTOM | DL_EDGE_LEFT:
    chunk->tiles.values[z * width * height + y * width + x].terrain = 28;
    break;
  case DL_EDGE_TOP | DL_EDGE_RIGHT | DL_EDGE_BOTTOM:
    chunk->tiles.values[z * width * height + y * width + x].terrain = 25;
    break;
  case DL_EDGE_TOP | DL_EDGE_RIGHT | DL_EDGE_LEFT:
    chunk->tiles.values[z * width * height + y * width + x].terrain = 31;
    break;
  case DL_EDGE_TOP | DL_EDGE_BOTTOM | DL_EDGE_LEFT:
    chunk->tiles.values[z * width * height + y * width + x].terrain = 27;
    break;
  case DL_EDGE_RIGHT | DL_EDGE_BOTTOM | DL_EDGE_LEFT:
    chunk->tiles.values[z * width * height + y * width + x].terrain = 24;
    break;
  case DL_EDGE_RIGHT | DL_EDGE_BOTTOM | DL_EDGE_LEFT | DL_EDGE_TOP:
    chunk->tiles.values[z * width * height + y * width + x].terrain = 23;
    break;
  }

  // tiles[z * width * height + y * width + x].terrain = terrain_id;
}

uint32_t MapGenerator::m_get_bitmask(const std::vector<int>& terrain, const int x, const int y, const int z)
{
  const int padded_width = width + m_generation_padding * 2;
  const int padded_height = height + m_generation_padding * 2;
  uint32_t bitmask = 0;

  // Top
  if (y > 0 && terrain[z * padded_width * padded_height + (y - 1) * padded_width + x] == 0)
  {
    bitmask |= DL_EDGE_TOP;
  }
  // Right
  if (x < padded_width - 1 && terrain[z * padded_width * padded_height + y * padded_width + x + 1] == 0)
  {
    bitmask |= DL_EDGE_RIGHT;
  }
  // Bottom
  if (y < padded_height - 1 && terrain[z * padded_width * padded_height + (y + 1) * padded_width + x] == 0)
  {
    bitmask |= DL_EDGE_BOTTOM;
  }
  // Left
  if (x > 0 && terrain[z * padded_width * padded_height + y * padded_width + x - 1] == 0)
  {
    bitmask |= DL_EDGE_LEFT;
  }

  return bitmask;
}

}  // namespace dl
