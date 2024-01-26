#pragma once

#include <entt/entity/fwd.hpp>
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
void save_world(World& world);
void load_world(World& world);
void save_game(World& world, entt::registry& registry);
void load_game(World& world, entt::registry& registry);

void save_terrain(const Grid3D& tiles, const std::string& file_name);
void load_chunk(Chunk& chunk, const std::string& file_name);
}  // namespace dl::serialization
