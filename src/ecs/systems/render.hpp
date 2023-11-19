#pragma once

#include <entt/entt.hpp>

namespace dl
{
class World;
class ViewCamera;
class Renderer;

class RenderSystem
{
 public:
  RenderSystem(World& world);
  void update(entt::registry& registry, Renderer& renderer, const ViewCamera& camera);

 private:
  World& m_world;
};
}  // namespace dl
