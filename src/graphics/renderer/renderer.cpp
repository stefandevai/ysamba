#include "./renderer.hpp"

#include <spdlog/spdlog.h>
#include <webgpu/wgpu.h>

#include "core/game_context.hpp"
#include "definitions.hpp"
#include "graphics/camera.hpp"
#include "graphics/display.hpp"
#include "graphics/renderer/utils.hpp"
#include "graphics/renderer/wgpu_context.hpp"

#ifdef DL_BUILD_DEBUG_TOOLS
#include "debug/debug_tools.hpp"
#endif

namespace dl
{
Renderer::Renderer(GameContext& game_context)
    : m_game_context(game_context), context(game_context.display->wgpu_context)
{
}

Renderer::~Renderer()
{
  // Release depth buffer
  if (m_has_loaded)
  {
    wgpuTextureViewRelease(depth_texture_view);
    wgpuTextureDestroy(depth_texture);
    wgpuTextureRelease(depth_texture);
  }
}

void Renderer::load()
{
  Shader shader{};
  shader.load(context.device, "data/shaders/default.wgsl");

  // Load descriptors used during rendering
  target_view_descriptor = {
      .label = "Target View",
      .dimension = WGPUTextureViewDimension_2D,
      .baseMipLevel = 0,
      .mipLevelCount = 1,
      .baseArrayLayer = 0,
      .arrayLayerCount = 1,
      .aspect = WGPUTextureAspect_All,
  };

  command_encoder_descriptor = {
      .label = "Command Encoder",
  };

  m_load_depth_buffer(context.device);

  main_pass.load(shader, depth_texture_view);

  m_has_loaded = true;
}

void Renderer::resize() { m_load_depth_buffer(context.device); }

void Renderer::m_load_depth_buffer(WGPUDevice device)
{
  if (m_has_loaded)
  {
    wgpuTextureViewRelease(depth_texture_view);
    wgpuTextureDestroy(depth_texture);
    wgpuTextureRelease(depth_texture);
  }

  const auto& display_size = Display::get_window_size();

  const auto depth_texture_format = WGPUTextureFormat_Depth24Plus;

  WGPUTextureDescriptor depthTextureDesc = {
      .dimension = WGPUTextureDimension_2D,
      .format = depth_texture_format,
      .mipLevelCount = 1,
      .sampleCount = 1,
      .size = {static_cast<uint32_t>(display_size.x), static_cast<uint32_t>(display_size.y), 1},
      .usage = WGPUTextureUsage_RenderAttachment,
      .viewFormatCount = 1,
      .viewFormats = &depth_texture_format,
  };

  depth_texture = wgpuDeviceCreateTexture(device, &depthTextureDesc);
  assert(depth_texture != nullptr);

  WGPUTextureViewDescriptor depthTextureViewDesc = {
      .aspect = WGPUTextureAspect_DepthOnly,
      .baseArrayLayer = 0,
      .arrayLayerCount = 1,
      .baseMipLevel = 0,
      .mipLevelCount = 1,
      .dimension = WGPUTextureViewDimension_2D,
      .format = depth_texture_format,
  };

  depth_texture_view = wgpuTextureCreateView(depth_texture, &depthTextureViewDesc);
  assert(depth_texture_view != nullptr);
}

void Renderer::render(const Camera& camera)
{
  WGPUSurfaceTexture surface_texture;
  wgpuSurfaceGetCurrentTexture(context.surface, &surface_texture);

  if (surface_texture.status == WGPUSurfaceGetCurrentTextureStatus_Timeout)
  {
    return;
  }

  if (surface_texture.status != WGPUSurfaceGetCurrentTextureStatus_Success)
  {
    spdlog::critical("Failed to get current surface texture");
    return;
  }

  target_view_descriptor.format = wgpuTextureGetFormat(surface_texture.texture);
  WGPUTextureView targetView = wgpuTextureCreateView(surface_texture.texture, &target_view_descriptor);
  WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(context.device, &command_encoder_descriptor);

  main_pass.render(targetView, encoder, camera);

#ifdef DL_BUILD_DEBUG_TOOLS
  // Debug tools render pass for ImGui
  WGPURenderPassColorAttachment debug_render_pass_color_attachment = {
      .view = targetView,
      .resolveTarget = nullptr,
      .loadOp = WGPULoadOp_Load,
      .storeOp = WGPUStoreOp_Store,
  };

  WGPURenderPassDescriptor debug_render_pass_descriptor = {
      .timestampWrites = nullptr,
      .depthStencilAttachment = nullptr,
      .colorAttachments = &debug_render_pass_color_attachment,
      .colorAttachmentCount = 1,
  };

  WGPURenderPassEncoder debug_render_pass = wgpuCommandEncoderBeginRenderPass(encoder, &debug_render_pass_descriptor);
  DebugTools::get_instance().update();
  DebugTools::get_instance().render(debug_render_pass);
  wgpuRenderPassEncoderEnd(debug_render_pass);
  wgpuRenderPassEncoderRelease(debug_render_pass);
#endif

  WGPUCommandBufferDescriptor commandBufferDescriptor = {
      .label = "Command Buffer",
  };
  auto commandBuffer = wgpuCommandEncoderFinish(encoder, &commandBufferDescriptor);
  wgpuQueueSubmit(context.queue, 1, &commandBuffer);

  wgpuCommandBufferRelease(commandBuffer);
  wgpuCommandEncoderRelease(encoder);
  wgpuTextureViewRelease(targetView);

  wgpuSurfacePresent(context.surface);
  wgpuTextureRelease(surface_texture.texture);
}

void Renderer::clear_color(const uint8_t r, const uint8_t g, const uint8_t b, const float a)
{
  main_pass.clear_color = {
      .r = static_cast<double>(r) / 255.0,
      .g = static_cast<double>(g) / 255.0,
      .b = static_cast<double>(b) / 255.0,
      .a = a,
  };
}

const Texture* Renderer::get_texture(const uint32_t resource_id)
{
  return m_game_context.asset_manager->get<Texture>(resource_id, context.device);
}

}  // namespace dl
