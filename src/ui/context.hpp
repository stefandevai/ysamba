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
class UIManager;
class UIComponent;
}  // namespace dl::ui

namespace dl::ui
{
struct UIContext
{
  UIManager* ui_manager;
  AssetManager* asset_manager;
  Renderer* renderer;
  Clock* clock;
  entt::registry* animator;
  std::vector<glm::mat4>* matrix_stack;
  std::vector<UIComponent*>* focused_stack;
  int min_z_index = 0;
  int max_z_index = 0;
};

}  // namespace dl::ui
