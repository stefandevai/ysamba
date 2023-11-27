#pragma once

#include <entt/entity/fwd.hpp>

namespace dl
{
class World;
class Camera;
class Renderer;

class RenderSystem
{
 public:
  RenderSystem(World& world);
  void render(entt::registry& registry, Renderer& renderer, const Camera& camera);

 private:
  World& m_world;
  const std::string& m_world_texture_id;
  const int m_frustum_tile_padding = 1;
};
}  // namespace dl
