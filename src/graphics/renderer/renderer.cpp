#include "./renderer.hpp"

#include <spdlog/spdlog.h>
#include <webgpu/webgpu.h>

#include "core/display.hpp"
#include "core/game_context.hpp"
#include "graphics/camera.hpp"

namespace dl::v2
{
Renderer::Renderer(GameContext& game_context) : m_game_context(game_context) {}

WGPURenderPipeline pipeline;

void Renderer::init()
{
  assert(m_game_context.display != nullptr);

  queue = wgpuDeviceGetQueue(m_game_context.display->device);

  auto on_queue_work_done = [](WGPUQueueWorkDoneStatus status, void*) {
    spdlog::debug("Queue work done. Status: {}", (uint32_t)status);
  };

  wgpuQueueOnSubmittedWorkDone(queue, on_queue_work_done, nullptr);

  // TEMP
  const char* shaderSource = R"(
@vertex
fn vs_main(@builtin(vertex_index) in_vertex_index: u32) -> @builtin(position) vec4<f32> {
	var p = vec2f(0.0, 0.0);
	if (in_vertex_index == 0u) {
		p = vec2f(-0.5, -0.5);
	} else if (in_vertex_index == 1u) {
		p = vec2f(0.5, -0.5);
	} else {
		p = vec2f(0.0, 0.5);
	}
	return vec4f(p, 0.0, 1.0);
}

@fragment
fn fs_main() -> @location(0) vec4f {
    return vec4f(0.0, 0.4, 1.0, 1.0);
}
)";

  WGPUShaderModuleDescriptor shaderDesc = {};
  shaderDesc.nextInChain = nullptr;
  shaderDesc.hintCount = 0;
  shaderDesc.hints = nullptr;

  // Use the extension mechanism to load a WGSL shader source code
  WGPUShaderModuleWGSLDescriptor shaderCodeDesc = {};
  // Set the chained struct's header
  shaderCodeDesc.chain.next = nullptr;
  shaderCodeDesc.chain.sType = WGPUSType_ShaderModuleWGSLDescriptor;
  // Connect the chain
  shaderDesc.nextInChain = &shaderCodeDesc.chain;

  // Setup the actual payload of the shader code descriptor
  shaderCodeDesc.code = shaderSource;

  WGPUShaderModule shaderModule = wgpuDeviceCreateShaderModule(m_game_context.display->device, &shaderDesc);

  WGPURenderPipelineDescriptor pipelineDesc = {};
  pipelineDesc.nextInChain = nullptr;
  pipelineDesc.vertex.bufferCount = 0;
  pipelineDesc.vertex.buffers = nullptr;

  // Vertex shader
  pipelineDesc.vertex.module = shaderModule;
  pipelineDesc.vertex.entryPoint = "vs_main";
  pipelineDesc.vertex.constantCount = 0;
  pipelineDesc.vertex.constants = nullptr;

  // Primitive assembly and rasterization
  pipelineDesc.primitive.topology = WGPUPrimitiveTopology_TriangleList;
  pipelineDesc.primitive.stripIndexFormat = WGPUIndexFormat_Undefined;
  pipelineDesc.primitive.frontFace = WGPUFrontFace_CCW;
  pipelineDesc.primitive.cullMode = WGPUCullMode_None;

  // Fragment shader
  WGPUFragmentState fragmentState = {};
  fragmentState.nextInChain = nullptr;
  pipelineDesc.fragment = &fragmentState;
  fragmentState.module = shaderModule;
  fragmentState.entryPoint = "fs_main";
  fragmentState.constantCount = 0;
  fragmentState.constants = nullptr;

  // Configure blend state
  WGPUBlendState blendState;
  blendState.color.srcFactor = WGPUBlendFactor_SrcAlpha;
  blendState.color.dstFactor = WGPUBlendFactor_OneMinusSrcAlpha;
  blendState.color.operation = WGPUBlendOperation_Add;
  blendState.alpha.srcFactor = WGPUBlendFactor_Zero;
  blendState.alpha.dstFactor = WGPUBlendFactor_One;
  blendState.alpha.operation = WGPUBlendOperation_Add;

  WGPUColorTargetState colorTarget = {};
  colorTarget.nextInChain = nullptr;
  colorTarget.format = m_game_context.display->surface_format;
  colorTarget.blend = &blendState;
  colorTarget.writeMask = WGPUColorWriteMask_All;

  // We have only one target because our render pass has only one output color
  // attachment.
  fragmentState.targetCount = 1;
  fragmentState.targets = &colorTarget;

  // Depth and stencil tests are not used here
  pipelineDesc.depthStencil = nullptr;

  // Multi-sampling
  pipelineDesc.multisample.count = 1;

  // Pipeline layout
  pipelineDesc.layout = nullptr;

  pipeline = wgpuDeviceCreateRenderPipeline(m_game_context.display->device, &pipelineDesc);
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

  wgpuRenderPassEncoderSetPipeline(renderPass, pipeline);
  wgpuRenderPassEncoderDraw(renderPass, 3, 1, 0, 0);

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
