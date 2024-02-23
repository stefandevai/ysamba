#pragma once

namespace dl
{
class AssetManager;
// class Batch;
struct Clock;

namespace v2
{
class Renderer;
}
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
  v2::Renderer* renderer;
  // Batch* batch;
  Clock* clock;
  entt::registry* animator;
  std::vector<glm::mat4>* matrix_stack;
};

}  // namespace dl::ui
