#pragma once

#include <vector>

#include "core/maths/vector.hpp"
#include "world/generators/biome_type.hpp"

namespace dl
{
struct Chunk;
struct WorldMetadata;

class ChunkGenerator
{
 public:
  Vector3i size{1, 1, 1};
  std::vector<float> vegetation_type;
  std::vector<float> vegetation_density;
  std::unique_ptr<Chunk> chunk = nullptr;

  // Quantity of tiles per map texture pixel
  double map_to_tiles = 16.0;

  ChunkGenerator(const WorldMetadata& world_metadata);

  void generate(const int seed, const Vector3i& offset = Vector3i{});
  void set_size(const Vector3i& size);

 private:
  enum Edge
  {
    DL_EDGE_NONE = 0,
    DL_EDGE_TOP = 1,
    DL_EDGE_RIGHT = 2,
    DL_EDGE_BOTTOM = 4,
    DL_EDGE_LEFT = 8,
    DL_EDGE_TOP_LEFT = 16,
    DL_EDGE_TOP_RIGHT = 32,
    DL_EDGE_BOTTOM_RIGHT = 64,
    DL_EDGE_BOTTOM_LEFT = 128,
  };

  enum class Sampler
  {
    Nearest,
    Bilinear,
    Bicubic,
  };

  int m_padding = 1;
  Vector3i m_padded_size{size.x + m_padding * 2, size.y + m_padding * 2, 1};
  const WorldMetadata& m_world_metadata;
  Sampler m_height_map_sampler = Sampler::Bicubic;

  void m_generate_noise_data(const int seed, const Vector3i& offset);

  Vector2 m_world_to_noise_map(const Vector3i& world_position);

  int m_sample_height_map(const Vector3i& world_position);

  BiomeType m_sample_biome(const Vector3i& world_position);

  void m_select_tile(const std::vector<int>& terrain, const int x, const int y, const int z);

  int m_select_decoration(const int terrain_id, const int x, const int y, const int z);

  uint32_t m_get_bitmask_4_sided(
      const std::vector<int>& terrain, const int x, const int y, const int z, const int neighbor);

  uint32_t m_get_bitmask_8_sided(
      const std::vector<int>& terrain, const int x, const int y, const int z, const int neighbor, const int source);

  bool m_has_neighbor(const std::vector<int>& terrain, const int x, const int y, const int z, const int neighbor);
};
}  // namespace dl
