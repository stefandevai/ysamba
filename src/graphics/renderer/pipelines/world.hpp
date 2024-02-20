#pragma once

#include <webgpu/webgpu.h>

#include "graphics/renderer/shader.hpp"

namespace dl
{
class WorldPipeline
{
 public:
  WGPURenderPipeline pipeline;

  WorldPipeline() = default;
  ~WorldPipeline();

  void load(const WGPUDevice device, const WGPUTextureFormat texture_format, const Shader& shader);
  void render(const WGPURenderPassEncoder render_pass);

 private:
  bool m_has_loaded = false;
  WGPUQueue m_queue;
};
}  // namespace dl
