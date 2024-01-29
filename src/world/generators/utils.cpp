#include "./utils.hpp"

#include "./island_data.hpp"

namespace dl::utils
{
const FastNoise::SmartNode<FastNoise::Remap> get_island_noise_generator(const IslandNoiseParams& params)
{
  // First noise layer
  const auto& simplex = FastNoise::New<FastNoise::OpenSimplex2S>();
  const auto& fractal = FastNoise::New<FastNoise::FractalFBm>();
  fractal->SetSource(simplex);
  fractal->SetOctaveCount(params.layer_1_octaves);
  fractal->SetLacunarity(params.layer_1_lacunarity);
  fractal->SetGain(params.layer_1_gain);
  fractal->SetWeightedStrength(params.layer_1_weighted_strength);

  const auto& distance_to_point = FastNoise::New<FastNoise::DistanceToPoint>();
  distance_to_point->SetDistanceFunction(FastNoise::DistanceFunction::EuclideanSquared);

  const auto& subtract = FastNoise::New<FastNoise::Subtract>();
  subtract->SetLHS(fractal);
  subtract->SetRHS(distance_to_point);

  const auto& terrace = FastNoise::New<FastNoise::Terrace>();
  terrace->SetMultiplier(params.layer_1_terrace_multiplier);
  terrace->SetSource(subtract);

  const auto remap = FastNoise::New<FastNoise::Remap>();
  remap->SetSource(terrace);
  remap->SetRemap(
      params.layer_1_remap_from.x, params.layer_1_remap_from.y, params.layer_1_remap_to.x, params.layer_1_remap_to.y);

  // Second noise layer
  const auto simplex2 = FastNoise::New<FastNoise::OpenSimplex2S>();
  const auto seed_offset = FastNoise::New<FastNoise::SeedOffset>();
  seed_offset->SetSource(simplex2);
  seed_offset->SetOffset(params.layer_2_seed_offset);

  const auto fractal2 = FastNoise::New<FastNoise::FractalFBm>();
  fractal2->SetSource(seed_offset);
  fractal2->SetOctaveCount(params.layer_2_octave_count);
  fractal2->SetLacunarity(params.layer_2_lacunarity);
  fractal2->SetGain(params.layer_2_gain);
  fractal2->SetWeightedStrength(params.layer_2_weighted_strength);

  const auto fade = FastNoise::New<FastNoise::Fade>();
  fade->SetA(subtract);
  fade->SetB(fractal2);
  fade->SetFade(params.layer_2_fade);

  const auto max_smooth = FastNoise::New<FastNoise::MaxSmooth>();
  max_smooth->SetLHS(fade);
  max_smooth->SetRHS(params.layer_2_smooth_rhs);
  max_smooth->SetSmoothness(params.layer_2_smoothness);

  // Convergence of both layers
  const auto min = FastNoise::New<FastNoise::Min>();
  min->SetLHS(remap);
  min->SetRHS(max_smooth);

  const auto remap2 = FastNoise::New<FastNoise::Remap>();
  remap2->SetSource(min);
  remap2->SetRemap(-1.0f, 1.0f, 0.0f, 1.00f);

  return remap2;
}
}  // namespace dl::utils
