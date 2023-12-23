#pragma once

#include <entt/entity/fwd.hpp>

namespace dl
{
class World;
class Camera;
class Renderer;
class Texture;
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

  Perspective perspective = TopDown90;

  RenderSystem(Renderer& renderer, World& world);
  void render(entt::registry& registry, const Camera& camera);

 private:
  Renderer& m_renderer;
  World& m_world;
  const std::string& m_world_texture_id;
  const std::shared_ptr<Texture> m_world_texture = nullptr;
  const int m_frustum_tile_padding = 1;

  template <typename T>
  void m_batch(const Position& position, T* renderable, const Vector2i& size, const int z_index = 0);

  void m_render_tile(const uint32_t tile_id,
                     const Vector2i& camera_position,
                     const Vector2i& tile_size,
                     const int x,
                     const int y,
                     const int z,
                     const int z_index = 0);
};
}  // namespace dl
