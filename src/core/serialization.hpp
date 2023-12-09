#pragma once

#include <entt/entity/fwd.hpp>

namespace dl
{
class World;
}

namespace dl::serialization
{
void save_world(World& world);
void load_world(World& world);
void save_game(World& world, entt::registry& registry);
void load_game(World& world, entt::registry& registry);
}  // namespace dl::serialization
