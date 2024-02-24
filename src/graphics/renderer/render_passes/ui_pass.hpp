#pragma once

#include <webgpu/wgpu.h>

#include "graphics/renderer/batch.hpp"

namespace dl
{
struct GameContext;
struct WGPUContext;
class Camera;

class UIPass
{
 private:
  GameContext& m_game_context;
  WGPUContext& m_context;

 public:
  Batch batch{m_game_context};
  WGPUColor clear_color{0.0, 0.0, 0.0, 1.0};
  WGPURenderPassDepthStencilAttachment depth_stencil_attachment;
  WGPURenderPassColorAttachment render_pass_color_attachment;
  WGPURenderPassDescriptor render_pass_descriptor;

  UIPass(GameContext& game_context);
  ~UIPass();

  // Delete copy/move assignment operator and copy/move constructor
  UIPass& operator=(const UIPass&) = delete;
  UIPass(const UIPass&) = delete;
  UIPass&& operator=(UIPass&& rhs) = delete;
  UIPass(UIPass&& rhs) = delete;

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
