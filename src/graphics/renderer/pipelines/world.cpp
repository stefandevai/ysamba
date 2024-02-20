#include "./world.hpp"

#include <array>

#include "graphics/renderer/mesh.hpp"
#include "graphics/renderer/utils.hpp"

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
uint32_t vertex_size;
uint32_t vertex_count;
WGPUBindGroupLayoutEntry binding_layout;
WGPUBindGroup bindGroup;
WGPUBuffer uniformBuffer;

struct Uniforms
{
  std::array<float, 4> color;
  float time;
  float _padding[3];
};
Uniforms uniforms{};
WGPUDepthStencilState stencil_state;
// TEMP

void WorldPipeline::load(const WGPUDevice device, const WGPUTextureFormat texture_format, const Shader& shader)
{
  m_queue = wgpuDeviceGetQueue(device);

  // Mesh
  mesh.load(device);

  // Uniforms
  uniforms.time = 1.0f;
  uniforms.color = {1.0f, 1.0f, 1.0f, 1.0f};

  WGPUBufferDescriptor bufferDesc = {};
  bufferDesc.nextInChain = nullptr;
  bufferDesc.size = sizeof(Uniforms);
  bufferDesc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform;
  bufferDesc.mappedAtCreation = false;
  uniformBuffer = wgpuDeviceCreateBuffer(device, &bufferDesc);
  wgpuQueueWriteBuffer(m_queue, uniformBuffer, 0, &uniforms, sizeof(Uniforms));

  binding_layout = utils::default_binding_layout();
  binding_layout.binding = 0;
  binding_layout.visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
  binding_layout.buffer.type = WGPUBufferBindingType_Uniform;
  binding_layout.buffer.minBindingSize = sizeof(Uniforms);

  WGPUBindGroupLayoutDescriptor bindGroupLayoutDesc{};
  bindGroupLayoutDesc.nextInChain = nullptr;
  bindGroupLayoutDesc.entryCount = 1;
  bindGroupLayoutDesc.entries = &binding_layout;
  WGPUBindGroupLayout bindGroupLayout = wgpuDeviceCreateBindGroupLayout(device, &bindGroupLayoutDesc);

  WGPUBindGroupEntry binding{};
  binding.nextInChain = nullptr;
  binding.binding = 0;
  binding.buffer = uniformBuffer;
  binding.offset = 0;
  binding.size = sizeof(Uniforms);

  WGPUBindGroupDescriptor bindGroupDesc{};
  bindGroupDesc.nextInChain = nullptr;
  bindGroupDesc.layout = bindGroupLayout;
  bindGroupDesc.entryCount = bindGroupLayoutDesc.entryCount;
  bindGroupDesc.entries = &binding;
  bindGroup = wgpuDeviceCreateBindGroup(device, &bindGroupDesc);

  // Pipeline
  WGPURenderPipelineDescriptor pipelineDesc = {};
  pipelineDesc.nextInChain = nullptr;

  // Uniforms layout
  WGPUPipelineLayoutDescriptor pipelineLayoutDesc = {};
  pipelineLayoutDesc.nextInChain = nullptr;
  pipelineLayoutDesc.bindGroupLayoutCount = 1;
  pipelineLayoutDesc.bindGroupLayouts = &bindGroupLayout;
  WGPUPipelineLayout pipelineLayout = wgpuDeviceCreatePipelineLayout(device, &pipelineLayoutDesc);
  pipelineDesc.layout = pipelineLayout;

  // Vertex fetch
  vertex_size = 6;
  vertex_count = mesh.count / vertex_size;
  std::array<WGPUVertexAttribute, 2> vertexAttribs{};
  vertexAttribs[0].shaderLocation = 0;
  vertexAttribs[0].format = WGPUVertexFormat_Float32x3;
  vertexAttribs[0].offset = 0;

  vertexAttribs[1].shaderLocation = 1;
  vertexAttribs[1].format = WGPUVertexFormat_Float32x3;
  vertexAttribs[1].offset = 3 * sizeof(float);

  WGPUVertexBufferLayout vertexBufferLayout = {};
  vertexBufferLayout.attributeCount = vertexAttribs.size();
  vertexBufferLayout.attributes = vertexAttribs.data();
  vertexBufferLayout.arrayStride = vertex_size * sizeof(float);
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

  stencil_state = utils::default_depth_stencil_state();
  stencil_state.depthCompare = WGPUCompareFunction_Less;
  stencil_state.depthWriteEnabled = true;
  stencil_state.format = WGPUTextureFormat_Depth24Plus;
  stencil_state.stencilReadMask = 0;
  stencil_state.stencilWriteMask = 0;
  pipelineDesc.depthStencil = &stencil_state;

  pipelineDesc.multisample.count = 1;

  pipeline = wgpuDeviceCreateRenderPipeline(device, &pipelineDesc);

  m_has_loaded = true;
}

void WorldPipeline::render(const WGPURenderPassEncoder render_pass)
{
  static int idx = 0;
  ++idx;
  uniforms.time = static_cast<float>(idx) * 0.05f;
  uniforms.color = {1.0f, 1.0f, 1.0f, 1.0f};

  wgpuQueueWriteBuffer(m_queue, uniformBuffer, offsetof(Uniforms, time), &uniforms.time, sizeof(Uniforms::time));
  wgpuQueueWriteBuffer(m_queue, uniformBuffer, offsetof(Uniforms, color), &uniforms.color, sizeof(Uniforms::color));

  wgpuRenderPassEncoderSetPipeline(render_pass, pipeline);
  wgpuRenderPassEncoderSetVertexBuffer(render_pass, 0, mesh.buffer, 0, mesh.size);
  wgpuRenderPassEncoderSetBindGroup(render_pass, 0, bindGroup, 0, nullptr);
  wgpuRenderPassEncoderDraw(render_pass, vertex_count, 1, 0, 0);
}
}  // namespace dl
