#include "./chunk_manager.hpp"

#include <spdlog/spdlog.h>

#include <cmath>

#include "core/maths/neighbor_iterator.hpp"

namespace dl
{
void ChunkManager::update(const Vector3i& target)
{
  Vector3i target_chunk_position{};
  target_chunk_position.x = std::floor(target.x / chunk_size.x) * chunk_size.x;
  target_chunk_position.y = std::floor(target.y / chunk_size.y) * chunk_size.y;
  target_chunk_position.z = std::floor(target.z / chunk_size.z) * chunk_size.z;

  if (!is_loaded(target_chunk_position))
  {
    load(target_chunk_position);
  }

  for (NeighborIterator<Vector3i> it = Vector3i{}; it.neighbor != 8; ++it)
  {
    const auto& neighbor_index = *it;

    Vector3i neighbor{};
    neighbor.x = target_chunk_position.x + chunk_size.x * neighbor_index.x;
    neighbor.y = target_chunk_position.y + chunk_size.y * neighbor_index.y;
    neighbor.z = target_chunk_position.z;

    if (!is_loaded(neighbor))
    {
      load(neighbor);
    }
  }

  std::erase_if(chunks, [this, &target_chunk_position](const auto& chunk) {
    return !is_within_radius(chunk->position, target_chunk_position, 3);
  });
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

bool ChunkManager::is_within_radius(const Vector3i& origin, const Vector3i& target, const int radius) const
{
  return std::abs(origin.x - target.x) < radius * chunk_size.x &&
         std::abs(origin.y - target.y) < radius * chunk_size.y && std::abs(origin.z - target.z) < radius * chunk_size.z;
}
}  // namespace dl
