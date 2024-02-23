#pragma once

namespace dl
{
class AssetManager;
struct Clock;
class Renderer;
}  // namespace dl

namespace dl::ui
{
class AnimationManager;
}

namespace dl::ui
{
struct UIContext
{
  AssetManager* asset_manager;
  Renderer* renderer;
  Clock* clock;
  entt::registry* animator;
  std::vector<glm::mat4>* matrix_stack;
};

}  // namespace dl::ui
