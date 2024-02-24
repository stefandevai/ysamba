#pragma once

#include <webgpu/wgpu.h>

#include "graphics/renderer/batch.hpp"

namespace dl
{
struct GameContext;
struct WGPUContext;
class Camera;

class MainRenderPass
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

  MainRenderPass(GameContext& game_context);
  ~MainRenderPass();

  // Delete copy/move assignment operator and copy/move constructor
  MainRenderPass& operator=(const MainRenderPass&) = delete;
  MainRenderPass(const MainRenderPass&) = delete;
  MainRenderPass&& operator=(MainRenderPass&& rhs) = delete;
  MainRenderPass(MainRenderPass&& rhs) = delete;

  void load(const Shader& shader, const WGPUTextureView depth_texture_view);
  void render(WGPUTextureView target_view, WGPUCommandEncoder encoder, const Camera& camera);
  void resize(const WGPUTextureView depth_texture_view);

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
