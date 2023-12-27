#include "./chunk_manager.hpp"

#include <spdlog/spdlog.h>

#include <cmath>

#include "core/maths/neighbor_iterator.hpp"

namespace dl
{
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

void ChunkManager::generate(const Vector3i& position)
{
  auto chunk = std::make_unique<Chunk>(position, true);
  chunks.push_back(std::move(chunk));
}

void ChunkManager::set_chunk_size(const Vector3i& chunk_size) { this->chunk_size = chunk_size; }

void ChunkManager::set_frustum(const Vector2i& frustum) { this->frustum = frustum; }

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
