#pragma once

#include <entt/entity/fwd.hpp>

namespace dl
{
class World;
class Camera;
class Renderer;
struct Position;
struct Vector2i;

class RenderSystem
{
 public:
  enum Perspective
  {
    TopDown90,
    TopDown45,
  };

  Perspective perspective = TopDown45;

  RenderSystem(World& world);
  void render(entt::registry& registry, Renderer& renderer, const Camera& camera);

 private:
  World& m_world;
  const std::string& m_world_texture_id;
  const int m_frustum_tile_padding = 1;

  template <typename T>
  void m_batch(
      Renderer& renderer, const Position& position, T* renderable, const Vector2i& size, const int z_index = 0);

  void m_render_tile(Renderer& renderer,
                     const uint32_t tile_id,
                     const Vector2i& camera_position,
                     const Vector2i& tile_size,
                     const int x,
                     const int y,
                     const int z,
                     const int z_index = 0);
};
}  // namespace dl
