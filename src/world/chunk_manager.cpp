#include "./chunk_manager.hpp"

#include <spdlog/spdlog.h>

#include <cmath>
#include <thread>

#include "./generators/game_chunk_generator.hpp"
#include "config.hpp"
#include "core/maths/neighbor_iterator.hpp"
#include "core/random.hpp"
#include "core/serialization.hpp"

// TEMP
#include <chrono>
// TEMP

namespace dl
{
Chunk ChunkManager::null = Chunk{};
std::mutex ChunkManager::m_chunks_to_add_mutex = std::mutex{};

ChunkManager::ChunkManager()
{
  m_seed = random::get_integer(0, 10000);
  m_thread_pool.initialize();
}

ChunkManager::~ChunkManager()
{
  while (m_thread_pool.is_busy())
  {
  }
  m_thread_pool.finalize();
}

void ChunkManager::load_or_generate(const Vector3i& position)
{
  if (serialization::chunk_exists(position))
  {
    load_sync(position);
    return;
  }
  else
  {
    generate_sync(position, config::chunk_size);
    return;
  }
}

void ChunkManager::load_initial_chunks(const Vector3i& target)
{
  const auto top_left_position = world_to_chunk(target.x - frustum.x / 2, target.y - frustum.y / 2, target.z);

  for (int j = top_left_position.y; j < target.y + frustum.y / 2; j += config::chunk_size.y)
  {
    for (int i = top_left_position.x; i < target.x + frustum.x / 2; i += config::chunk_size.y)
    {
      const auto& candidate = world_to_chunk(i, j, target.z);

      if (!is_loaded(candidate))
      {
        // load_sync(candidate);
        load_or_generate(candidate);
      }
    }
  }
}

void ChunkManager::update(const Vector3i& target)
{
  const int padding = 1;

  {
    // Load visible chunks
    // const auto top_left_position = world_to_chunk(
    //     target.x - frustum.x / 2 - padding * config::chunk_size.x, target.y - frustum.y / 2 - padding *
    //     config::chunk_size.y, target.z);
    const auto top_left_position =
        world_to_chunk(target.x - padding * config::chunk_size.x, target.y - padding * config::chunk_size.y, target.z);

    const auto bottom_right_position = world_to_chunk(target.x + frustum.x + padding * config::chunk_size.x,
                                                      target.y + frustum.y + padding * config::chunk_size.y,
                                                      target.z);

    for (int j = top_left_position.y; j <= bottom_right_position.y; j += config::chunk_size.y)
    {
      for (int i = top_left_position.x; i <= bottom_right_position.x; i += config::chunk_size.y)
      {
        const auto& candidate = world_to_chunk(i, j, target.z);

        if (!is_loaded(candidate))
        {
          // load_async(candidate);
          // load_sync(candidate);
          load_or_generate(candidate);
        }
      }
    }
  }

  {
    const auto target_chunk_position = world_to_chunk(target);

    // Activate / Deactivate chunks within a certain radius
    // activate_if([this, &target_chunk_position](const auto& chunk) {
    //   return is_within_tile_distance(chunk->position, target_chunk_position, Vector2i{frustum.x * 2, frustum.y * 2});
    // });

    // Unload chunks within a certain radius
    std::erase_if(chunks, [this, &target_chunk_position](const auto& chunk) {
      return !is_within_tile_distance(
          chunk->position,
          target_chunk_position,
          Vector2i{frustum.x + 2 * config::chunk_size.x, frustum.y + 2 * config::chunk_size.y});
    });
  }

  {
    // Incorporate newly loaded / generated chunks
    const std::unique_lock<std::mutex> lock(m_chunks_to_add_mutex);
    if (m_chunks_to_add.size() > 0)
    {
      for (auto& chunk : m_chunks_to_add)
      {
        std::erase_if(m_chunks_loading, [&chunk](const auto& position) { return chunk->position == position; });
      }
      chunks.insert(chunks.end(),
                    std::make_move_iterator(m_chunks_to_add.begin()),
                    std::make_move_iterator(m_chunks_to_add.end()));
      m_chunks_to_add.clear();
    }
  }
}

bool ChunkManager::is_loaded(const Vector3i& position) const
{
  const auto found = std::find_if(
      chunks.begin(), chunks.end(), [&position](const auto& chunk) { return chunk->position == position; });

  const auto found_generating =
      std::find_if(m_chunks_loading.begin(), m_chunks_loading.end(), [&position](const auto& chunk_position) {
        return position == chunk_position;
      });

  return found != chunks.end() || found_generating != m_chunks_loading.end();
}

void ChunkManager::load_async(const Vector3i& position)
{
  const auto found =
      std::find_if(m_chunks_loading.begin(), m_chunks_loading.end(), [&position](const Vector3i& generating_position) {
        return generating_position == position;
      });

  if (found == m_chunks_loading.end())
  {
    m_thread_pool.queue_job([this, position, size = config::chunk_size] {
      generate_async(std::ref(position), std::ref(size), std::ref(m_chunks_to_add_mutex));
    });
    m_chunks_loading.push_back(position);
  }
}

void ChunkManager::generate_async(const Vector3i& position, const Vector3i& size, std::mutex& mutex)
{
  GameChunkGenerator generator{};
  generator.set_size(size);
  generator.generate(m_seed, position);
  // auto chunk = std::make_unique<Chunk>(position, true);
  // chunk->tiles.set_size(size);
  // chunk->tiles.values = std::move(generator.tiles);
  // chunk->tiles.height_map = std::move(generator.height_map);
  // chunk->tiles.compute_visibility();

  {
    const std::unique_lock<std::mutex> lock(mutex);
    m_chunks_to_add.push_back(std::move(generator.chunk));
  }
}

void ChunkManager::load_sync(const Vector3i& position)
{
  // Timer timer{};
  // timer.start();

  auto chunk = std::make_unique<Chunk>(position, true);
  chunk->tiles.set_size(config::chunk_size);
  serialization::load_game_chunk(*chunk, "test2.world");

  // spdlog::debug("Chunk size: {} {} {}", chunk->tiles.size.x, chunk->tiles.size.y, chunk->tiles.size.z);
  // timer.stop();
  // timer.print(fmt::format("Chunk ({}, {}, {})", position.x, position.y, position.z));

  if (chunk->tiles.height_map.size() != static_cast<uint32_t>(config::chunk_size.x * config::chunk_size.y))
  {
    spdlog::critical("Could not load chunk: invalid height map size");
    return;
  }

  chunks.push_back(std::move(chunk));
}

void ChunkManager::generate_sync(const Vector3i& position, const Vector3i& size)
{
  GameChunkGenerator generator{};
  generator.set_size(size);
  generator.generate(m_seed, position);
  serialization::save_game_chunk(*generator.chunk, "test2.world");
  chunks.push_back(std::move(generator.chunk));
}

void ChunkManager::set_frustum(const Vector2i& frustum) { this->frustum = frustum; }

Chunk& ChunkManager::at(const int x, const int y, const int z) const
{
  const Vector3i chunk_position = world_to_chunk(x, y, z);

  const auto chunk = std::find_if(chunks.begin(), chunks.end(), [&chunk_position](const auto& chunk) {
    return (chunk->position == chunk_position);
  });

  /* assert(chunk != chunks.end() && "Chunk should be already generated during update"); */

  if (chunk != chunks.end())
  {
    return *(*chunk).get();
  }

  /* spdlog::warn("Should't be generating a chunk here"); */

  return ChunkManager::null;
}

Chunk& ChunkManager::at(const Vector3i& position) const { return at(position.x, position.y, position.z); }

Chunk& ChunkManager::in(const int x, const int y, const int z) const
{
  const Vector3i chunk_position = world_to_chunk(x, y, z);

  const auto chunk = std::find_if(chunks.begin(), chunks.end(), [&chunk_position](const auto& chunk) {
    return (chunk->position == chunk_position);
  });

  if (chunk != chunks.end())
  {
    return *(*chunk).get();
  }

  return ChunkManager::null;
}

Chunk& ChunkManager::in(const Vector3i& position) const { return in(position.x, position.y, position.z); }

Vector3i ChunkManager::world_to_chunk(const int x, const int y, const int z) const
{
  return Vector3i{std::floor(x / static_cast<float>(config::chunk_size.x)) * config::chunk_size.x,
                  std::floor(y / static_cast<float>(config::chunk_size.y)) * config::chunk_size.y,
                  std::floor(z / static_cast<float>(config::chunk_size.z)) * config::chunk_size.z};
}

Vector3i ChunkManager::world_to_chunk(const Vector3i& position) const
{
  return world_to_chunk(position.x, position.y, position.z);
}

bool ChunkManager::is_within_tile_radius(const Vector3i& origin, const Vector3i& target, const int radius) const
{
  return std::abs(origin.x - target.x) <= radius && std::abs(origin.y - target.y) <= radius &&
         std::abs(origin.z - target.z) <= radius;
}

bool ChunkManager::is_within_tile_distance(const Vector3i& origin,
                                           const Vector3i& target,
                                           const Vector2i& distance) const
{
  return std::abs(origin.x - target.x) <= distance.x && std::abs(origin.y - target.y) <= distance.y;
}

bool ChunkManager::is_within_chunk_radius(const Vector3i& origin, const Vector3i& target, const int radius) const
{
  return std::abs(origin.x - target.x) <= radius * config::chunk_size.x &&
         std::abs(origin.y - target.y) <= radius * config::chunk_size.y &&
         std::abs(origin.z - target.z) <= radius * config::chunk_size.z;
}

void ChunkManager::activate_if(const std::function<bool(const std::unique_ptr<Chunk>&)>& condition)
{
  for (auto& chunk : chunks)
  {
    if (condition(chunk))
    {
      chunk->active = true;
    }
    else
    {
      chunk->active = false;
    }
  }
}

}  // namespace dl
