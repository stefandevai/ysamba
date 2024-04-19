#pragma once

#include <functional>
#include <mutex>
#include <vector>

#include "./chunk.hpp"
#include "core/maths/vector.hpp"
#include "core/thread_pool.hpp"
#include "definitions.hpp"

#ifdef DL_BUILD_DEBUG_TOOLS
#include "world/generators/island_data.hpp"
#endif

namespace dl
{
struct GameContext;
struct WorldMetadata;

class ChunkManager
{
 public:
#ifdef DL_BUILD_DEBUG_TOOLS
  enum class Mode
  {
    // Save, load and generate chunks
    Default,
    // Only generates chunks
    NoLoadingOrSaving,
  };
  Mode mode = Mode::Default;
  IslandNoiseParams island_params{};

  void regenerate_chunks();
#endif

  std::vector<std::unique_ptr<Chunk>> chunks{};
  // Frustum in tiles
  Vector2i frustum{80, 48};

  static Chunk null;

  ChunkManager(GameContext& game_context);
  ~ChunkManager();

  // Update chunks based on a tile position
  void update(const Vector3i& target);
  void load_or_generate(const Vector3i& position);
  void load_initial_chunks(const Vector3i& position);
  void load_async(const Vector3i& position);
  void load_sync(const Vector3i& position);
  void generate_async(const Vector3i& position, const Vector3i& size, std::mutex& mutex);
  void generate_sync(const Vector3i& position, const Vector3i& size);
  void set_frustum(const Vector2i& frustum);

  // Gets chunk at a precise location
  Chunk& at(const int x, const int y, const int z) const;
  Chunk& at(const Vector3i& position) const;

  // Gets chunk that contains a position
  Chunk& in(const int x, const int y, const int z) const;
  Chunk& in(const Vector3i& position) const;

  Vector3i world_to_chunk(const int x, const int y, const int z) const;
  Vector3i world_to_chunk(const Vector3i& position) const;

  bool is_loaded(const Vector3i& position) const;
  bool is_within_tile_radius(const Vector3i& origin, const Vector3i& target, const int radius) const;
  bool is_within_tile_distance(const Vector3i& origin, const Vector3i& target, const Vector2i& distance) const;
  bool is_within_chunk_radius(const Vector3i& origin, const Vector3i& target, const int radius) const;
  void activate_if(const std::function<bool(const std::unique_ptr<Chunk>&)>& condition);

 private:
  GameContext& m_game_context;
  const WorldMetadata& m_world_metadata;
  std::vector<Vector3i> m_chunks_loading{};
  std::vector<std::unique_ptr<Chunk>> m_chunks_to_add{};
  static std::mutex m_chunks_to_add_mutex;
  ThreadPool m_thread_pool{};
  int m_seed = 0;
};
};  // namespace dl
