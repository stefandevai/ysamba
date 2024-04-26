#include "./utils.hpp"

#include "./island_data.hpp"

namespace dl::utils
{
void generate_silhouette_map(
    float* data, int x, int y, int width, int height, const IslandNoiseParams& params, int seed)
{
  const auto simplex = FastNoise::New<FastNoise::OpenSimplex2S>();
  const auto fractal = FastNoise::New<FastNoise::FractalFBm>();
  fractal->SetSource(simplex);
  fractal->SetOctaveCount(params.layer_1_octaves);
  fractal->SetLacunarity(2.52f);
  fractal->SetGain(0.68f);
  fractal->SetWeightedStrength(0.6f);

  const auto remap = FastNoise::New<FastNoise::Remap>();
  remap->SetSource(fractal);
  remap->SetRemap(-1.0f, 1.0f, 0.0f, 1.0f);

  remap->GenUniformGrid2D(data, x, y, width, height, params.frequency, seed);
  // remap->GenUniformGrid2D(data, x, y, width, height, 0.016f, seed);
}

void generate_mountain_map(float* data, int x, int y, int width, int height, const IslandNoiseParams& params, int seed)
{
  const auto simplex = FastNoise::New<FastNoise::OpenSimplex2S>();
  const auto fractal = FastNoise::New<FastNoise::FractalRidged>();
  fractal->SetSource(simplex);
  fractal->SetOctaveCount(4);
  fractal->SetLacunarity(1.59f);
  fractal->SetGain(1.18f);
  fractal->SetWeightedStrength(0.8f);

  // const auto remap = FastNoise::New<FastNoise::Remap>();
  // remap->SetSource(fractal);
  // remap->SetRemap(-1.0f, 1.0f, 0.0f, 1.0f);

  fractal->GenUniformGrid2D(data, x, y, width, height, params.frequency, seed);
}

void generate_control_map(float* data, int x, int y, int width, int height, const IslandNoiseParams& params, int seed)
{
  const auto simplex = FastNoise::New<FastNoise::OpenSimplex2S>();
  const auto fractal = FastNoise::New<FastNoise::FractalFBm>();
  fractal->SetSource(simplex);
  fractal->SetOctaveCount(2);
  fractal->SetLacunarity(2.19f);
  fractal->SetGain(2.18f);
  fractal->SetWeightedStrength(0.0f);

  const auto remap = FastNoise::New<FastNoise::Remap>();
  remap->SetSource(fractal);
  remap->SetRemap(-1.0f, 1.0f, 0.0f, 1.0f);

  remap->GenUniformGrid2D(data, x, y, width, height, params.frequency, seed);
}

void generate_humidity_map(float* data, int width, int height, const IslandNoiseParams& params, int seed)
{
  const auto simplex = FastNoise::New<FastNoise::OpenSimplex2S>();
  const auto fractal = FastNoise::New<FastNoise::FractalFBm>();
  fractal->SetSource(simplex);
  fractal->SetOctaveCount(2);
  fractal->SetLacunarity(2.19f);
  fractal->SetGain(0.5f);
  fractal->SetWeightedStrength(0.8f);

  const auto remap = FastNoise::New<FastNoise::Remap>();
  remap->SetSource(fractal);
  remap->SetRemap(-1.0f, 1.0f, 0.0f, 1.0f);

  remap->GenUniformGrid2D(data, 0, 0, width, height, params.frequency - 0.003f, seed);
}

void generate_temperature_map(float* data, int width, int height, const IslandNoiseParams& params, int seed)
{
  const auto simplex = FastNoise::New<FastNoise::OpenSimplex2S>();
  const auto fractal = FastNoise::New<FastNoise::FractalFBm>();
  fractal->SetSource(simplex);
  fractal->SetOctaveCount(2);
  fractal->SetLacunarity(1.19f);
  fractal->SetGain(0.5f);
  fractal->SetWeightedStrength(1.8f);

  const auto remap = FastNoise::New<FastNoise::Remap>();
  remap->SetSource(fractal);
  remap->SetRemap(-1.0f, 1.0f, 0.0f, 1.0f);

  remap->GenUniformGrid2D(data, 0, 0, width, height, params.frequency, seed);
}

void generate_height_modifier_map(float* data, int x, int y, int width, int height, int seed)
{
  const auto simplex = FastNoise::New<FastNoise::OpenSimplex2S>();
  const auto fractal = FastNoise::New<FastNoise::FractalFBm>();
  fractal->SetSource(simplex);
  fractal->SetOctaveCount(7);
  fractal->SetLacunarity(1.19f);
  fractal->SetGain(0.5f);
  fractal->SetWeightedStrength(1.8f);

  const auto remap = FastNoise::New<FastNoise::Remap>();
  remap->SetSource(fractal);
  remap->SetRemap(-1.0f, 1.0f, 0.0f, 1.0f);

  remap->GenUniformGrid2D(data, x, y, width, height, 0.056, seed);
}
}  // namespace dl::utils
