#include "./renderer.hpp"

#include <spdlog/spdlog.h>
#include <webgpu/webgpu.h>

#include "core/display.hpp"
#include "core/game_context.hpp"
#include "graphics/camera.hpp"
#include "graphics/renderer/pipelines/world.hpp"
#include "graphics/renderer/shader.hpp"

namespace dl::v2
{
Renderer::Renderer(GameContext& game_context) : m_game_context(game_context) {}

Shader shader{};
WorldPipeline world_pipeline{};

void Renderer::init()
{
  assert(m_game_context.display != nullptr);

  queue = wgpuDeviceGetQueue(m_game_context.display->device);

  auto on_queue_work_done = [](WGPUQueueWorkDoneStatus status, void*) {
    spdlog::debug("Queue work done. Status: {}", (uint32_t)status);
  };

  wgpuQueueOnSubmittedWorkDone(queue, on_queue_work_done, nullptr);

  // TEMP
  shader.load(m_game_context.display->device, "data/shaders/default.wgsl");
  world_pipeline.load(m_game_context.display->device, m_game_context.display->surface_format, shader);
  // TEMP
}

void Renderer::render(const Camera& camera)
{
  WGPUSurfaceTexture surface_texture;
  wgpuSurfaceGetCurrentTexture(m_game_context.display->surface, &surface_texture);

  if (surface_texture.status == WGPUSurfaceGetCurrentTextureStatus_Timeout)
  {
    return;
  }

  if (surface_texture.status != WGPUSurfaceGetCurrentTextureStatus_Success)
  {
    spdlog::critical("Failed to get current surface texture");
    return;
  }

  WGPUTextureViewDescriptor descriptor = {};
  descriptor.nextInChain = nullptr;
  descriptor.label = "Target View";
  descriptor.format = wgpuTextureGetFormat(surface_texture.texture);
  descriptor.dimension = WGPUTextureViewDimension_2D;
  descriptor.baseMipLevel = 0;
  descriptor.mipLevelCount = 1;
  descriptor.baseArrayLayer = 0;
  descriptor.arrayLayerCount = 1;
  descriptor.aspect = WGPUTextureAspect_All;
  WGPUTextureView targetView = wgpuTextureCreateView(surface_texture.texture, &descriptor);

  WGPUCommandEncoderDescriptor commandEncoderDesc = {};
  commandEncoderDesc.nextInChain = nullptr;
  commandEncoderDesc.label = "Command Encoder";
  WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(m_game_context.display->device, &commandEncoderDesc);

  WGPURenderPassDescriptor renderPassDesc = {};
  WGPURenderPassColorAttachment renderPassColorAttachment = {};
  renderPassColorAttachment.view = targetView;
  renderPassColorAttachment.resolveTarget = nullptr;
  renderPassColorAttachment.loadOp = WGPULoadOp_Clear;
  renderPassColorAttachment.storeOp = WGPUStoreOp_Store;
  renderPassColorAttachment.clearValue = WGPUColor{0.9, 0.1, 0.2, 1.0};
  renderPassDesc.colorAttachmentCount = 1;
  renderPassDesc.colorAttachments = &renderPassColorAttachment;
  renderPassDesc.depthStencilAttachment = nullptr;
  renderPassDesc.timestampWrites = nullptr;
  renderPassDesc.nextInChain = nullptr;
  WGPURenderPassEncoder renderPass = wgpuCommandEncoderBeginRenderPass(encoder, &renderPassDesc);

  world_pipeline.render(renderPass);

  wgpuRenderPassEncoderEnd(renderPass);
  wgpuRenderPassEncoderRelease(renderPass);

  WGPUCommandBufferDescriptor commandBufferDescriptor = {};
  commandBufferDescriptor.nextInChain = nullptr;
  commandBufferDescriptor.label = "Command Buffer";
  auto commandBuffer = wgpuCommandEncoderFinish(encoder, &commandBufferDescriptor);
  wgpuQueueSubmit(queue, 1, &commandBuffer);

  wgpuCommandEncoderRelease(encoder);
  wgpuTextureViewRelease(targetView);

  wgpuSurfacePresent(m_game_context.display->surface);
  wgpuTextureRelease(surface_texture.texture);
}

void Renderer::clear_color(const uint8_t r, const uint8_t g, const uint8_t b, const float a) {}
}  // namespace dl::v2
