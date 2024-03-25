#pragma once

#include <entt/entity/fwd.hpp>
#include <unordered_map>

#include "ecs/components/tile.hpp"

namespace dl
{
class World;
class Camera;
struct Chunk;
class Batch;
class Renderer;
struct GameContext;
struct Vector2i;
struct Vector3i;

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
  const Vector2i& m_tile_size;
  std::unordered_map<uint32_t, Tile> m_tiles{};
  static constexpr int m_frustum_tile_padding = 1;
  static constexpr double m_z_index_increment = 0.02;

  void m_render_map_tiles(const Camera& camera);
  void m_render_map_tile(const Chunk& chunk, const uint32_t tile_id, const Vector3i& position, const int z_index = 0);

  void m_create_sprite(entt::registry& registry, entt::entity entity);

  friend class RenderEditor;
};
}  // namespace dl
