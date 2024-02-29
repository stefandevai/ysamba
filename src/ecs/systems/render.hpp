#pragma once

#include <entt/entity/fwd.hpp>
#include <string>
#include <unordered_map>

#include "ecs/components/tile.hpp"

namespace dl
{
class World;
class Camera;
struct Position;
struct Vector2i;
struct Chunk;
class Batch;
class Texture;
class Renderer;
struct GameContext;

class RenderSystem
{
 public:
  RenderSystem(GameContext& game_context, World& world);
  void render(entt::registry& registry, const Camera& camera);

 private:
  GameContext& m_game_context;
  Renderer& m_renderer;
  Batch& m_batch;
  World& m_world;
  std::unordered_map<uint32_t, Tile> m_tiles{};
  const uint32_t m_world_texture_id;
  const Texture* m_world_texture = nullptr;
  static constexpr int m_frustum_tile_padding = 1;
  static constexpr double m_z_index_increment = 0.02;

  void m_render_tiles(const Camera& camera);
  void m_render_tile(const Chunk& chunk,
                     const uint32_t tile_id,
                     const Vector2i& tile_size,
                     const int x,
                     const int y,
                     const int z,
                     const int z_index = 0);

  void m_create_sprite(entt::registry& registry, entt::entity entity);

  friend class RenderEditor;
};
}  // namespace dl
