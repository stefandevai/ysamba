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
  Vector3i chunk_size{32, 32, 32};
  // Frustum in tiles
  Vector2i frustum{80, 48};

  ChunkManager() = default;

  // Update chunks based on a tile position
  void update(const Vector3i& target);
  void load(const Vector3i& position);
  void generate(const Vector3i& position);
  void set_chunk_size(const Vector3i& chunk_size);
  void set_frustum(const Vector2i& frustum);

  bool is_loaded(const Vector3i& position) const;
  bool is_within_tile_radius(const Vector3i& origin, const Vector3i& target, const int radius) const;
  bool is_within_chunk_radius(const Vector3i& origin, const Vector3i& target, const int radius) const;
  void activate_if(const std::function<bool(const std::unique_ptr<Chunk>&)>& condition);
};
};  // namespace dl
