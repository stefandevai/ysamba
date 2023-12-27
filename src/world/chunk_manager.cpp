#include "./chunk_manager.hpp"

#include <spdlog/spdlog.h>

#include <cmath>

#include "core/maths/neighbor_iterator.hpp"

namespace dl
{
Chunk ChunkManager::null = Chunk{};

ChunkManager::ChunkManager() { m_generator.set_size(chunk_size); }

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
      Vector3i chunk_position{std::floor(i / chunk_size.x) * chunk_size.x,
                              std::floor(j / chunk_size.y) * chunk_size.y,
                              std::floor(target.z / chunk_size.z) * chunk_size.z};

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
      return !is_within_chunk_radius(chunk->position, target_chunk_position, 6);
    });
  }
}

bool ChunkManager::is_loaded(const Vector3i& position) const
{
  const auto found = std::find_if(
      chunks.begin(), chunks.end(), [&position](const auto& chunk) { return chunk->position == position; });

  return found != chunks.end();
}

void ChunkManager::load(const Vector3i& position) { generate(position); }

Chunk& ChunkManager::generate(const Vector3i& position)
{
  m_generator.generate(1337, position);
  auto chunk = std::make_unique<Chunk>(position, true);
  chunk->tiles.set_size(chunk_size);
  chunk->tiles.values = std::move(m_generator.tiles);
  chunk->tiles.height_map = std::move(m_generator.height_map);
  chunk->tiles.compute_visibility();

  spdlog::debug("CHUNK POSITION: {} {} {}", chunk->position.x, chunk->position.y, chunk->position.z);

  for (auto j = 0; j < 32; ++j)
  {
    for (auto i = 0; i < 32; ++i)
    {
      printf("%d ", chunk->tiles.height_map[j * 32 + i]);
    }
    printf("\n");
  }
  printf("\n\n");

  chunks.push_back(std::move(chunk));

  return *chunks[chunks.size() - 1].get();
}

void ChunkManager::set_chunk_size(const Vector3i& chunk_size)
{
  this->chunk_size = chunk_size;
  m_generator.set_size(chunk_size);
}

void ChunkManager::set_frustum(const Vector2i& frustum) { this->frustum = frustum; }

Chunk& ChunkManager::at(const int x, const int y, const int z) const
{
  /* const Vector3i chunk_position{std::floor(x / chunk_size.x) * chunk_size.x, */
  /*                               std::floor(y / chunk_size.y) * chunk_size.y, */
  /*                               std::floor(z / chunk_size.z) * chunk_size.z}; */
  Vector3i chunk_position{};

  if (x >= 0)
  {
    chunk_position.x = std::floor(x / (float)chunk_size.x) * chunk_size.x;
  }
  else
  {
    chunk_position.x = std::ceil(x / (float)chunk_size.x) * chunk_size.x;
  }
  if (y >= 0)
  {
    chunk_position.y = std::floor(y / (float)chunk_size.y) * chunk_size.y;
  }
  else
  {
    chunk_position.y = std::ceil(y / (float)chunk_size.y) * chunk_size.y;
  }
  if (z >= 0)
  {
    chunk_position.z = std::floor(z / (float)chunk_size.z) * chunk_size.z;
  }
  else
  {
    chunk_position.z = std::ceil(z / (float)chunk_size.z) * chunk_size.z;
  }

  const auto chunk = std::find_if(chunks.begin(), chunks.end(), [&chunk_position](auto& chunk) {
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

Chunk& ChunkManager::at(const Vector3i& position) const
{
  return at(position.x, position.y, position.z);
}

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

uint32_t ChunkManager::index_at(const Vector3i& position) const
{
  return index_at(position.x, position.y, position.z);
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
