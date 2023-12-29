#include "./chunk_manager.hpp"

#include <spdlog/spdlog.h>

#include <cmath>
#include <thread>

#include "./generators/map_generator.hpp"
#include "core/maths/neighbor_iterator.hpp"

namespace dl
{
Chunk ChunkManager::null = Chunk{};
std::mutex ChunkManager::m_chunks_to_add_mutex = std::mutex{};

ChunkManager::ChunkManager() { m_thread_pool.initialize(); }

ChunkManager::~ChunkManager()
{
  while (m_thread_pool.is_busy())
  {
  }
  m_thread_pool.finalize();
}

void ChunkManager::update(const Vector3i& target)
{
  // Load visible chunks
  const int padding = 1;

  for (int j = target.y - padding * chunk_size.y - frustum.y / 2;
       j <= target.y + frustum.y / 2 + padding * chunk_size.y;
       j += chunk_size.y)
  {
    for (int i = target.x - padding * chunk_size.x - frustum.x / 2;
         i <= target.x + frustum.x / 2 + padding * chunk_size.x;
         i += chunk_size.x)
    {
      Vector3i chunk_position{std::floor(i / static_cast<float>(chunk_size.x)) * chunk_size.x,
                              std::floor(j / static_cast<float>(chunk_size.y)) * chunk_size.y,
                              std::floor(target.z / static_cast<float>(chunk_size.z)) * chunk_size.z};

      if (!is_loaded(chunk_position))
      {
        load(chunk_position);
      }
    }
  }

  {
    Vector3i target_chunk_position{std::floor(target.x / chunk_size.x) * chunk_size.x,
                                   std::floor(target.y / chunk_size.y) * chunk_size.y,
                                   std::floor(target.z / chunk_size.z) * chunk_size.z};

    // Activate / Deactivate chunks within a certain radius
    activate_if([this, &target_chunk_position](const auto& chunk) {
      return is_within_chunk_radius(chunk->position, target_chunk_position, 3);
    });

    // Unload chunks within a certain radius
    std::erase_if(chunks, [this, &target_chunk_position](const auto& chunk) {
      return !is_within_chunk_radius(chunk->position, target_chunk_position, 4);
    });
  }

  {
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

  const auto found_generating = std::find_if(m_chunks_loading.begin(),
                                             m_chunks_loading.end(),
                                             [&position](auto chunk_position) { return position == chunk_position; });

  return found != chunks.end() || found_generating != m_chunks_loading.end();
}

void ChunkManager::load(const Vector3i& position)
{
  const auto found =
      std::find_if(m_chunks_loading.begin(), m_chunks_loading.end(), [&position](Vector3i& generating_position) {
        return generating_position == position;
      });

  if (found == m_chunks_loading.end())
  {
    m_thread_pool.queue_job([this, position, size = this->chunk_size] {
      generate(std::ref(position), std::ref(size), std::ref(m_chunks_to_add_mutex));
    });
    m_chunks_loading.push_back(position);
  }
}

void ChunkManager::generate(const Vector3i& position, const Vector3i& size, std::mutex& mutex)
{
  MapGenerator generator{};
  generator.set_size(size);
  generator.generate(1337, position);
  auto chunk = std::make_unique<Chunk>(position, true);
  chunk->tiles.set_size(size);
  chunk->tiles.values = std::move(generator.tiles);
  chunk->tiles.height_map = std::move(generator.height_map);
  chunk->tiles.compute_visibility();

  {
    const std::unique_lock<std::mutex> lock(mutex);
    m_chunks_to_add.push_back(std::move(chunk));
  }
}

void ChunkManager::set_chunk_size(const Vector3i& chunk_size) { this->chunk_size = chunk_size; }

void ChunkManager::set_frustum(const Vector2i& frustum) { this->frustum = frustum; }

Chunk& ChunkManager::at(const int x, const int y, const int z) const
{
  const Vector3i chunk_position{std::floor(x / static_cast<float>(chunk_size.x)) * chunk_size.x,
                                std::floor(y / static_cast<float>(chunk_size.y)) * chunk_size.y,
                                std::floor(z / static_cast<float>(chunk_size.z)) * chunk_size.z};

  const auto chunk = std::find_if(
      chunks.begin(), chunks.end(), [&chunk_position](auto& chunk) { return (chunk->position == chunk_position); });

  /* assert(chunk != chunks.end() && "Chunk should be already generated during update"); */

  if (chunk != chunks.end())
  {
    return *(*chunk).get();
  }

  /* spdlog::warn("Should't be generating a chunk here"); */

  return ChunkManager::null;
}

Chunk& ChunkManager::at(const Vector3i& position) const { return at(position.x, position.y, position.z); }

uint32_t ChunkManager::index_at(const int x, const int y, const int z) const
{
  for (uint32_t i = 0; i < chunks.size(); ++i)
  {
    if (chunks[i]->position.x == x && chunks[i]->position.y == y && chunks[i]->position.z == z)
    {
      return i;
    }
  }

  /* assert(true && "Chunk should be already generated during update"); */

  return chunks.size();
}

uint32_t ChunkManager::index_at(const Vector3i& position) const { return index_at(position.x, position.y, position.z); }

Vector3i ChunkManager::world_to_chunk(const Vector3i& position) const
{
  return Vector3i{std::floor(position.x / static_cast<float>(chunk_size.x)) * chunk_size.x,
                  std::floor(position.y / static_cast<float>(chunk_size.y)) * chunk_size.y,
                  std::floor(position.z / static_cast<float>(chunk_size.z)) * chunk_size.z};
}

bool ChunkManager::is_within_tile_radius(const Vector3i& origin, const Vector3i& target, const int radius) const
{
  return std::abs(origin.x - target.x) <= radius && std::abs(origin.y - target.y) <= radius &&
         std::abs(origin.z - target.z) <= radius;
}

bool ChunkManager::is_within_chunk_radius(const Vector3i& origin, const Vector3i& target, const int radius) const
{
  return std::abs(origin.x - target.x) <= radius * chunk_size.x &&
         std::abs(origin.y - target.y) <= radius * chunk_size.y &&
         std::abs(origin.z - target.z) <= radius * chunk_size.z;
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
