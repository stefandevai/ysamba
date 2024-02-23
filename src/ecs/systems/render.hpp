#pragma once

#include <entt/entity/fwd.hpp>
#include <string>
#include <unordered_map>

#include "graphics/tile_render_data.hpp"

namespace dl
{
class World;
class Camera;
struct Position;
struct Vector2i;
struct Chunk;
class Batch;

namespace v2
{
class Texture;
class Renderer;
}  // namespace v2

class RenderSystem
{
 public:
  RenderSystem(v2::Renderer& renderer, World& world);
  void render(entt::registry& registry, const Camera& camera);

 private:
  v2::Renderer& m_renderer;
  Batch* m_batch = nullptr;
  World& m_world;
  std::unordered_map<uint32_t, TileRenderData> m_tiles{};
  const uint32_t m_world_texture_id;
  const v2::Texture* m_world_texture = nullptr;
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

  friend class RenderEditor;
};
}  // namespace dl
