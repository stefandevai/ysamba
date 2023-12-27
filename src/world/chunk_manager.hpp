#pragma once

#include <vector>

#include "./chunk.hpp"
#include "core/maths/vector.hpp"

namespace dl
{
class ChunkManager
{
 public:
  std::vector<std::unique_ptr<Chunk>> chunks{};
  // Size in tiles
  Vector3i chunk_size{4, 4, 4};
  // Frustum in tiles
  Vector2i frustum{3, 3};

  ChunkManager() = default;

  // Update chunks based on a tile position
  void update(const Vector3i& target);
  void load(const Vector3i& position);
  void generate(const Vector3i& position);
  void set_chunk_size(const Vector3i& chunk_size);

  bool is_loaded(const Vector3i& position) const;
  bool is_within_radius(const Vector3i& origin, const Vector3i& target, const int radius) const;
};
};  // namespace dl
