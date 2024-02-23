#pragma once

#include <webgpu/wgpu.h>

#include "graphics/renderer/batch.hpp"

namespace dl
{
struct WGPUContext;
class Camera;

class MainPass
{
 private:
  WGPUContext& m_context;

 public:
  Batch batch{m_context};
  WGPUColor clear_color{0.0, 0.0, 0.0, 1.0};
  WGPURenderPassDepthStencilAttachment depth_stencil_attachment;
  WGPURenderPassColorAttachment render_pass_color_attachment;
  WGPURenderPassDescriptor render_pass_descriptor;

  MainPass(WGPUContext& context);
  ~MainPass();

  // Delete copy/move assignment operator and copy/move constructor
  MainPass& operator=(const MainPass&) = delete;
  MainPass(const MainPass&) = delete;
  MainPass&& operator=(MainPass&& rhs) = delete;
  MainPass(MainPass&& rhs) = delete;

  void load(const Shader& shader, const WGPUTextureView depth_texture_view);
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
