#include "./world.hpp"

#include <spdlog/spdlog.h>

#include "graphics/camera.hpp"
#include "graphics/renderer/utils.hpp"

namespace dl
{
WorldPipeline::~WorldPipeline()
{
  if (m_has_loaded)
  {
    wgpuBufferDestroy(uniformBuffer);
    wgpuBufferRelease(uniformBuffer);
    wgpuBindGroupRelease(bindGroup);
    wgpuBindGroupLayoutRelease(bindGroupLayout);
    wgpuSamplerRelease(sampler);
    wgpuTextureViewRelease(textureView);
    wgpuTextureDestroy(texture);
    wgpuTextureRelease(texture);
    wgpuPipelineLayoutRelease(pipelineLayout);
    wgpuRenderPipelineRelease(pipeline);
  }
}

void WorldPipeline::load(const WGPUDevice device, const WGPUTextureFormat texture_format, const Shader& shader)
{
  m_queue = wgpuDeviceGetQueue(device);

  // Mesh
  mesh.load(device);

  uint32_t texture_width = 0;
  uint32_t texture_height = 0;

  {
    // Texture
    WGPUTextureDescriptor textureDesc{};
    textureDesc.label = "WorldPipeline Texture";
    textureDesc.nextInChain = nullptr;
    textureDesc.dimension = WGPUTextureDimension_2D;
    textureDesc.size = {64, 64, 1};
    textureDesc.mipLevelCount = 1;
    textureDesc.sampleCount = 1;
    textureDesc.format = WGPUTextureFormat_RGBA8Unorm;
    textureDesc.usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst;
    textureDesc.viewFormatCount = 0;
    textureDesc.viewFormats = nullptr;
    texture = wgpuDeviceCreateTexture(device, &textureDesc);
    texture_width = textureDesc.size.width;
    texture_height = textureDesc.size.height;

    std::vector<uint8_t> pixels(4 * texture_width * texture_height, 0u);
    for (uint32_t i = 0; i < texture_width; ++i)
    {
      for (uint32_t j = 0; j < texture_height; ++j)
      {
        pixels[4 * (j * texture_width + i) + 0] = 100u + (uint8_t)(i);
        pixels[4 * (j * texture_width + i) + 1] = 100u + (uint8_t)(j);
        pixels[4 * (j * texture_width + i) + 2] = 0u;
        pixels[4 * (j * texture_width + i) + 3] = 255u;
      }
    }

    WGPUImageCopyTexture destination;
    destination.texture = texture;
    destination.mipLevel = 0;
    destination.origin = {0, 0, 0};
    destination.aspect = WGPUTextureAspect_All;

    WGPUTextureDataLayout source;
    source.offset = 0;
    source.bytesPerRow = 4 * texture_width;
    source.rowsPerImage = texture_height;

    wgpuQueueWriteTexture(
        m_queue, &destination, pixels.data(), pixels.size() * sizeof(uint8_t), &source, &textureDesc.size);

    WGPUTextureViewDescriptor textureViewDesc{};
    textureViewDesc.label = "WorldPipeline Texture View";
    textureViewDesc.nextInChain = nullptr;
    textureViewDesc.aspect = WGPUTextureAspect_All;
    textureViewDesc.baseArrayLayer = 0;
    textureViewDesc.arrayLayerCount = 1;
    textureViewDesc.baseMipLevel = 0;
    textureViewDesc.mipLevelCount = 1;
    textureViewDesc.dimension = WGPUTextureViewDimension_2D;
    textureViewDesc.format = textureDesc.format;
    textureView = wgpuTextureCreateView(texture, &textureViewDesc);
  }

  // Sampler
  {
    WGPUSamplerDescriptor samplerDesc{};
    samplerDesc.label = "WorldPipeline Sampler";
    samplerDesc.addressModeU = WGPUAddressMode_ClampToEdge;
    samplerDesc.addressModeV = WGPUAddressMode_ClampToEdge;
    samplerDesc.addressModeW = WGPUAddressMode_ClampToEdge;
    samplerDesc.magFilter = WGPUFilterMode_Linear;
    samplerDesc.minFilter = WGPUFilterMode_Linear;
    samplerDesc.mipmapFilter = WGPUMipmapFilterMode_Linear;
    samplerDesc.lodMinClamp = 0.0f;
    samplerDesc.lodMaxClamp = 1.0f;
    samplerDesc.compare = WGPUCompareFunction_Undefined;
    samplerDesc.maxAnisotropy = 1;
    sampler = wgpuDeviceCreateSampler(device, &samplerDesc);
  }

  // Uniforms
  {
    WGPUBufferDescriptor bufferDesc{};
    bufferDesc.label = "WorldPipeline Uniform Buffer";
    bufferDesc.nextInChain = nullptr;
    bufferDesc.size = uniform_data.size;
    bufferDesc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform;
    bufferDesc.mappedAtCreation = false;
    uniformBuffer = wgpuDeviceCreateBuffer(device, &bufferDesc);

    const auto identity_matrix = glm::mat4(1.0f);
    wgpuQueueWriteBuffer(m_queue,
                         uniformBuffer,
                         uniform_data.projection_matrix_offset,
                         &identity_matrix,
                         uniform_data.projection_matrix_size);
    wgpuQueueWriteBuffer(
        m_queue, uniformBuffer, uniform_data.view_matrix_offset, &identity_matrix, uniform_data.view_matrix_size);

    binding_layout[0] = utils::default_binding_layout();
    binding_layout[0].binding = 0;
    binding_layout[0].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
    binding_layout[0].buffer.type = WGPUBufferBindingType_Uniform;
    binding_layout[0].buffer.minBindingSize = uniform_data.size;

    binding_layout[1] = utils::default_binding_layout();
    binding_layout[1].binding = 1;
    binding_layout[1].visibility = WGPUShaderStage_Fragment;
    binding_layout[1].texture.sampleType = WGPUTextureSampleType_Float;
    binding_layout[1].texture.viewDimension = WGPUTextureViewDimension_2D;

    binding_layout[2] = utils::default_binding_layout();
    binding_layout[2].binding = 2;
    binding_layout[2].visibility = WGPUShaderStage_Fragment;
    binding_layout[2].sampler.type = WGPUSamplerBindingType_Filtering;

    WGPUBindGroupLayoutDescriptor bindGroupLayoutDesc{};
    bindGroupLayoutDesc.nextInChain = nullptr;
    bindGroupLayoutDesc.entryCount = (uint32_t)binding_layout.size();
    bindGroupLayoutDesc.entries = binding_layout.data();
    bindGroupLayout = wgpuDeviceCreateBindGroupLayout(device, &bindGroupLayoutDesc);

    binding[0].nextInChain = nullptr;
    binding[0].binding = 0;
    binding[0].buffer = uniformBuffer;
    binding[0].offset = 0;
    binding[0].size = uniform_data.size;

    binding[1].nextInChain = nullptr;
    binding[1].binding = 1;
    binding[1].textureView = textureView;

    binding[2].nextInChain = nullptr;
    binding[2].binding = 2;
    binding[2].sampler = sampler;

    WGPUBindGroupDescriptor bindGroupDesc{};
    bindGroupDesc.nextInChain = nullptr;
    bindGroupDesc.layout = bindGroupLayout;
    bindGroupDesc.entryCount = (uint32_t)binding.size();
    bindGroupDesc.entries = binding.data();
    bindGroup = wgpuDeviceCreateBindGroup(device, &bindGroupDesc);
  }

  // Pipeline
  {
    WGPURenderPipelineDescriptor pipelineDesc{};
    pipelineDesc.nextInChain = nullptr;

    // Uniforms layout
    WGPUPipelineLayoutDescriptor pipelineLayoutDesc{};
    pipelineLayoutDesc.nextInChain = nullptr;
    pipelineLayoutDesc.bindGroupLayoutCount = 1;
    pipelineLayoutDesc.bindGroupLayouts = &bindGroupLayout;
    pipelineLayout = wgpuDeviceCreatePipelineLayout(device, &pipelineLayoutDesc);
    pipelineDesc.layout = pipelineLayout;

    // Vertex fetch
    vertex_size = 8;
    vertex_count = mesh.count / vertex_size;
    std::array<WGPUVertexAttribute, 3> vertexAttribs{};
    vertexAttribs[0].shaderLocation = 0;
    vertexAttribs[0].format = WGPUVertexFormat_Float32x3;
    vertexAttribs[0].offset = 0;

    vertexAttribs[1].shaderLocation = 2;
    vertexAttribs[1].format = WGPUVertexFormat_Float32x2;
    vertexAttribs[1].offset = 3 * sizeof(float);

    vertexAttribs[2].shaderLocation = 1;
    vertexAttribs[2].format = WGPUVertexFormat_Float32x3;
    vertexAttribs[2].offset = 5 * sizeof(float);

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
  }

  m_has_loaded = true;
}

void WorldPipeline::render(const WGPURenderPassEncoder render_pass, const Camera& camera)
{
  wgpuQueueWriteBuffer(m_queue,
                       uniformBuffer,
                       uniform_data.projection_matrix_offset,
                       &camera.projection_matrix,
                       uniform_data.projection_matrix_size);
  wgpuQueueWriteBuffer(
      m_queue, uniformBuffer, uniform_data.view_matrix_offset, &camera.view_matrix, uniform_data.view_matrix_size);

  wgpuRenderPassEncoderSetPipeline(render_pass, pipeline);
  wgpuRenderPassEncoderSetVertexBuffer(render_pass, 0, mesh.buffer, 0, mesh.size);
  wgpuRenderPassEncoderSetBindGroup(render_pass, 0, bindGroup, 0, nullptr);
  wgpuRenderPassEncoderDraw(render_pass, vertex_count, 1, 0, 0);
}
}  // namespace dl
