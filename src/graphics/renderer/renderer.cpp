#include "./renderer.hpp"

#include <spdlog/spdlog.h>
#include <webgpu/wgpu.h>

#include "core/display.hpp"
#include "core/game_context.hpp"
#include "graphics/camera.hpp"

namespace dl::v2
{
Renderer::Renderer(GameContext& game_context) : m_game_context(game_context) {}

Renderer::~Renderer()
{
  if (m_has_loaded)
  {
    wgpuTextureViewRelease(depth_texture_view);
    wgpuTextureDestroy(depth_texture);
    wgpuTextureRelease(depth_texture);
  }
}

void Renderer::load()
{
  assert(m_game_context.display != nullptr);

  queue = wgpuDeviceGetQueue(m_game_context.display->device);
  m_load_depth_buffer();

  // auto on_queue_work_done = [](WGPUQueueWorkDoneStatus status, void*) {
  //   spdlog::debug("Queue work done. Status: {}", (uint32_t)status);
  // };
  //
  // wgpuQueueOnSubmittedWorkDone(queue, on_queue_work_done, nullptr);

  // TEMP
  shader.load(m_game_context.display->device, "data/shaders/default.wgsl");
  world_pipeline.load(m_game_context.display->device, m_game_context.display->surface_format, shader);
  // TEMP
  m_has_loaded = true;
}

void Renderer::resize() { m_load_depth_buffer(); }

void Renderer::m_load_depth_buffer()
{
  if (m_has_loaded)
  {
    wgpuTextureViewRelease(depth_texture_view);
    wgpuTextureDestroy(depth_texture);
    wgpuTextureRelease(depth_texture);
  }

  const auto& display_size = Display::get_window_size();

  WGPUTextureFormat depth_texture_format = WGPUTextureFormat_Depth24Plus;
  WGPUTextureDescriptor depthTextureDesc;
  depthTextureDesc.dimension = WGPUTextureDimension_2D;
  depthTextureDesc.format = depth_texture_format;
  depthTextureDesc.mipLevelCount = 1;
  depthTextureDesc.sampleCount = 1;
  depthTextureDesc.size = {static_cast<uint32_t>(display_size.x), static_cast<uint32_t>(display_size.y), 1};
  depthTextureDesc.usage = WGPUTextureUsage_RenderAttachment;
  depthTextureDesc.viewFormatCount = 1;
  depthTextureDesc.viewFormats = &depth_texture_format;
  depth_texture = wgpuDeviceCreateTexture(m_game_context.display->device, &depthTextureDesc);

  WGPUTextureViewDescriptor depthTextureViewDesc;
  depthTextureViewDesc.aspect = WGPUTextureAspect_DepthOnly;
  depthTextureViewDesc.baseArrayLayer = 0;
  depthTextureViewDesc.arrayLayerCount = 1;
  depthTextureViewDesc.baseMipLevel = 0;
  depthTextureViewDesc.mipLevelCount = 1;
  depthTextureViewDesc.dimension = WGPUTextureViewDimension_2D;
  depthTextureViewDesc.format = depth_texture_format;
  depth_texture_view = wgpuTextureCreateView(depth_texture, &depthTextureViewDesc);
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

  WGPUTextureViewDescriptor descriptor{};
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

  WGPUCommandEncoderDescriptor commandEncoderDesc{};
  commandEncoderDesc.nextInChain = nullptr;
  commandEncoderDesc.label = "Command Encoder";
  WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(m_game_context.display->device, &commandEncoderDesc);

  WGPURenderPassDescriptor renderPassDesc{};
  WGPURenderPassColorAttachment renderPassColorAttachment{};
  renderPassColorAttachment.view = targetView;
  renderPassColorAttachment.resolveTarget = nullptr;
  renderPassColorAttachment.loadOp = WGPULoadOp_Clear;
  renderPassColorAttachment.storeOp = WGPUStoreOp_Store;
  // renderPassColorAttachment.clearValue = WGPUColor{0.28125, 0.44921875, 0.09375, 1.0};
  renderPassColorAttachment.clearValue = WGPUColor{0.0, 0.0, 0.0, 1.0};
  renderPassDesc.colorAttachmentCount = 1;
  renderPassDesc.colorAttachments = &renderPassColorAttachment;

  WGPURenderPassDepthStencilAttachment depthStencilAttachment;
  depthStencilAttachment.view = depth_texture_view;
  depthStencilAttachment.depthClearValue = 1.0f;
  depthStencilAttachment.depthLoadOp = WGPULoadOp_Clear;
  depthStencilAttachment.depthStoreOp = WGPUStoreOp_Store;
  depthStencilAttachment.depthReadOnly = false;
  depthStencilAttachment.stencilClearValue = 0;
  depthStencilAttachment.stencilLoadOp = WGPULoadOp_Clear;
  depthStencilAttachment.stencilStoreOp = WGPUStoreOp_Store;
  depthStencilAttachment.stencilReadOnly = true;
  renderPassDesc.depthStencilAttachment = &depthStencilAttachment;

  renderPassDesc.timestampWrites = nullptr;
  renderPassDesc.nextInChain = nullptr;
  WGPURenderPassEncoder renderPass = wgpuCommandEncoderBeginRenderPass(encoder, &renderPassDesc);

  world_pipeline.render(renderPass, camera);

  wgpuRenderPassEncoderEnd(renderPass);
  wgpuRenderPassEncoderRelease(renderPass);

  WGPUCommandBufferDescriptor commandBufferDescriptor = {};
  commandBufferDescriptor.nextInChain = nullptr;
  commandBufferDescriptor.label = "Command Buffer";
  auto commandBuffer = wgpuCommandEncoderFinish(encoder, &commandBufferDescriptor);
  wgpuQueueSubmit(queue, 1, &commandBuffer);

  wgpuCommandBufferRelease(commandBuffer);
  wgpuCommandEncoderRelease(encoder);
  wgpuTextureViewRelease(targetView);

  wgpuSurfacePresent(m_game_context.display->surface);
  wgpuTextureRelease(surface_texture.texture);
}

void Renderer::clear_color(const uint8_t r, const uint8_t g, const uint8_t b, const float a) {}
}  // namespace dl::v2
