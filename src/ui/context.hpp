#pragma once

namespace dl
{
class Batch;
class Renderer;
class AssetManager;
class Batch;
struct Clock;
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
  Batch* batch;
  Clock* clock;
  entt::registry* animator;
  std::vector<glm::mat4>* matrix_stack;
};

}  // namespace dl::ui
