#include "./world.hpp"

#include "graphics/renderer/mesh.hpp"

namespace dl
{
WorldPipeline::~WorldPipeline()
{
  if (m_has_loaded)
  {
    wgpuRenderPipelineRelease(pipeline);
  }
}

// TEMP
Mesh mesh{};
// TEMP

void WorldPipeline::load(const WGPUDevice device, const WGPUTextureFormat texture_format, const Shader& shader)
{
  mesh.load(device);
  WGPURenderPipelineDescriptor pipelineDesc = {};
  pipelineDesc.nextInChain = nullptr;

  // Vertex fetch
  WGPUVertexAttribute vertexAttrib;
  vertexAttrib.shaderLocation = 0;
  vertexAttrib.format = WGPUVertexFormat_Float32x2;
  vertexAttrib.offset = 0;

  WGPUVertexBufferLayout vertexBufferLayout = {};
  vertexBufferLayout.attributeCount = 1;
  vertexBufferLayout.attributes = &vertexAttrib;
  vertexBufferLayout.arrayStride = 2 * sizeof(float);
  vertexBufferLayout.stepMode = WGPUVertexStepMode_Vertex;

  pipelineDesc.vertex.bufferCount = 1;
  pipelineDesc.vertex.buffers = &vertexBufferLayout;

  // Vertex shader
  pipelineDesc.vertex.module = shader.module;
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
  fragmentState.module = shader.module;
  fragmentState.entryPoint = "fs_main";
  fragmentState.constantCount = 0;
  fragmentState.constants = nullptr;

  pipelineDesc.fragment = &fragmentState;

  // Blend state
  WGPUBlendState blendState;
  blendState.color.srcFactor = WGPUBlendFactor_SrcAlpha;
  blendState.color.dstFactor = WGPUBlendFactor_OneMinusSrcAlpha;
  blendState.color.operation = WGPUBlendOperation_Add;
  blendState.alpha.srcFactor = WGPUBlendFactor_Zero;
  blendState.alpha.dstFactor = WGPUBlendFactor_One;
  blendState.alpha.operation = WGPUBlendOperation_Add;

  WGPUColorTargetState colorTarget = {};
  colorTarget.nextInChain = nullptr;
  colorTarget.format = texture_format;
  colorTarget.blend = &blendState;
  colorTarget.writeMask = WGPUColorWriteMask_All;

  fragmentState.targetCount = 1;
  fragmentState.targets = &colorTarget;
  pipelineDesc.depthStencil = nullptr;
  pipelineDesc.multisample.count = 1;
  pipelineDesc.layout = nullptr;
  pipeline = wgpuDeviceCreateRenderPipeline(device, &pipelineDesc);

  m_has_loaded = true;
}

void WorldPipeline::render(const WGPURenderPassEncoder render_pass)
{
  wgpuRenderPassEncoderSetPipeline(render_pass, pipeline);
  wgpuRenderPassEncoderSetVertexBuffer(render_pass, 0, mesh.buffer, 0, mesh.size);
  wgpuRenderPassEncoderDraw(render_pass, mesh.count, 1, 0, 0);
}
}  // namespace dl
