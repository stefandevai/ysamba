#pragma once

#include <entt/entity/fwd.hpp>
#include <string>
#include <unordered_map>

#include "graphics/tile_render_data.hpp"

namespace dl
{
class World;
class Camera;
class Renderer;
class Texture;
struct Position;
struct Vector2i;
struct Chunk;
class Batch;

class RenderSystem
{
 public:
  RenderSystem(Renderer& renderer, World& world);
  void render(entt::registry& registry, const Camera& camera);

 private:
  Renderer& m_renderer;
  Batch* m_batch = nullptr;
  World& m_world;
  std::unordered_map<uint32_t, TileRenderData> m_tiles{};
  const std::string& m_world_texture_id;
  const std::shared_ptr<Texture> m_world_texture = nullptr;
  static constexpr int m_frustum_tile_padding = 1;
  static constexpr double m_z_index_increment = 0.02;

  /* template <typename T> */
  /* void m_batch(const Position& position, T* renderable, const Vector2i& size, const int z_index = 0); */

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
