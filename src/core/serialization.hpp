#pragma once

#include <entt/entity/fwd.hpp>
#include <filesystem>
#include <string>

namespace dl
{
class World;
class Grid3D;
struct Vector3i;
struct Chunk;
}  // namespace dl

namespace dl::serialization
{
void initialize_directories();

void save_world(World& world);
void load_world(World& world);
void save_game(World& world, entt::registry& registry);
void load_game(World& world, entt::registry& registry);

bool chunk_exists(const Vector3i& position, const std::string& world_id);
void save_game_chunk(const Chunk& chunk, const std::string& world_id);
void load_game_chunk(Chunk& chunk, const std::string& world_id);
}  // namespace dl::serialization
