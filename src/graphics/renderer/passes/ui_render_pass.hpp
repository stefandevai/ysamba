#pragma once

#include <webgpu/wgpu.h>

#include "graphics/renderer/batch.hpp"

namespace dl
{
struct GameContext;
struct WGPUContext;
class Camera;

class UIRenderPass
{
 private:
  GameContext& m_game_context;
  WGPUContext& m_context;

 public:
  Batch batch{m_game_context};
  WGPUColor clear_color{0.0, 0.0, 0.0, 1.0};
  WGPURenderPassColorAttachment render_pass_color_attachment;
  WGPURenderPassDescriptor render_pass_descriptor;

  UIRenderPass(GameContext& game_context);
  ~UIRenderPass();

  // Delete copy/move assignment operator and copy/move constructor
  UIRenderPass& operator=(const UIRenderPass&) = delete;
  UIRenderPass(const UIRenderPass&) = delete;
  UIRenderPass&& operator=(UIRenderPass&& rhs) = delete;
  UIRenderPass(UIRenderPass&& rhs) = delete;

  void load(const Shader& shader);
  void render(WGPUTextureView target_view, WGPUCommandEncoder encoder, const Camera& camera);

 private:
  enum BindGroupType
  {
    BIND_GROUP_UNIFORMS,
    BIND_GROUP_TEXTURES,
  };

  bool m_has_loaded = false;

  void m_update_texture_bind_group();
};

}  // namespace dl
