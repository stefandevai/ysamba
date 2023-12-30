#pragma once

#include <mutex>
#include <vector>

#include "./chunk.hpp"
#include "core/maths/vector.hpp"
#include "core/thread_pool.hpp"

namespace dl
{
class ChunkManager
{
 public:
  std::vector<std::unique_ptr<Chunk>> chunks{};
  // Size in tiles
  Vector3i chunk_size{32, 32, 10};
  // Frustum in tiles
  Vector2i frustum{80, 48};

  static Chunk null;

  ChunkManager();
  ~ChunkManager();

  // Update chunks based on a tile position
  void update(const Vector3i& target);
  void load(const Vector3i& position);
  void generate(const Vector3i& position, const Vector3i& size, std::mutex& mutex);
  void set_chunk_size(const Vector3i& chunk_size);
  void set_frustum(const Vector2i& frustum);
  Chunk& at(const int x, const int y, const int z) const;
  Chunk& at(const Vector3i& position) const;
  uint32_t index_at(const int x, const int y, const int z) const;
  uint32_t index_at(const Vector3i& position) const;
  Vector3i world_to_chunk(const Vector3i& position) const;

  bool is_loaded(const Vector3i& position) const;
  bool is_within_tile_radius(const Vector3i& origin, const Vector3i& target, const int radius) const;
  bool is_within_chunk_radius(const Vector3i& origin, const Vector3i& target, const int radius) const;
  void activate_if(const std::function<bool(const std::unique_ptr<Chunk>&)>& condition);

 private:
  std::vector<Vector3i> m_chunks_loading{};
  std::vector<std::unique_ptr<Chunk>> m_chunks_to_add{};
  static std::mutex m_chunks_to_add_mutex;
  ThreadPool m_thread_pool{};
};
};  // namespace dl