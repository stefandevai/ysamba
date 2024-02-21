#include "./world.hpp"

#include <spdlog/spdlog.h>

#include "graphics/camera.hpp"
#include "graphics/quad.hpp"
#include "graphics/renderer/texture.hpp"
#include "graphics/renderer/utils.hpp"
#include "graphics/renderer/wgpu_context.hpp"
#include "graphics/sprite.hpp"
#include "graphics/text.hpp"

namespace dl::v2
{
WorldPipeline::WorldPipeline(WGPUContext& context) : m_context(context) {}

WorldPipeline::~WorldPipeline()
{
  if (m_has_loaded)
  {
    wgpuBufferDestroy(uniformBuffer);
    wgpuBufferRelease(uniformBuffer);
    wgpuBindGroupRelease(bindGroup);
    wgpuBindGroupLayoutRelease(bindGroupLayout);
    wgpuSamplerRelease(sampler);
    wgpuPipelineLayoutRelease(pipelineLayout);
    wgpuRenderPipelineRelease(pipeline);
  }
}

Texture texture{"data/textures/tileset.png"};
Texture texture2{"data/textures/characters.png"};
WGPUBindGroupEntryExtras texture_view_entry{};
WGPUBindGroupLayoutEntryExtras texture_view_layout_entry{};

void WorldPipeline::load(const Shader& shader)
{
  // Mesh
  mesh.load(m_context.device);

  // Texture
  texture.load(m_context.device);
  texture2.load(m_context.device);

  // Sampler
  {
    WGPUSamplerDescriptor samplerDesc = {
        .label = "WorldPipeline Sampler",
        .addressModeU = WGPUAddressMode_ClampToEdge,
        .addressModeV = WGPUAddressMode_ClampToEdge,
        .addressModeW = WGPUAddressMode_ClampToEdge,
        .magFilter = WGPUFilterMode_Nearest,
        .minFilter = WGPUFilterMode_Linear,
        .mipmapFilter = WGPUMipmapFilterMode_Linear,
        .lodMinClamp = 0.0f,
        .lodMaxClamp = 1.0f,
        .compare = WGPUCompareFunction_Undefined,
        .maxAnisotropy = 1,
    };

    sampler = wgpuDeviceCreateSampler(m_context.device, &samplerDesc);
    assert(sampler != nullptr);
  }

  // Uniforms
  {
    WGPUBufferDescriptor bufferDesc = {
        .label = "WorldPipeline Uniform Buffer",
        .size = uniform_data.size,
        .usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform,
        .mappedAtCreation = false,
    };

    uniformBuffer = wgpuDeviceCreateBuffer(m_context.device, &bufferDesc);
    assert(uniformBuffer != nullptr);

    const auto identity_matrix = glm::mat4(1.0f);

    wgpuQueueWriteBuffer(m_context.queue,
                         uniformBuffer,
                         uniform_data.projection_matrix_offset,
                         &identity_matrix,
                         uniform_data.projection_matrix_size);
    wgpuQueueWriteBuffer(m_context.queue,
                         uniformBuffer,
                         uniform_data.view_matrix_offset,
                         &identity_matrix,
                         uniform_data.view_matrix_size);

    binding_layout[0] = {
      .binding = 0,
      .visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment,
      .buffer = {
        .type = WGPUBufferBindingType_Uniform,
        .minBindingSize = uniform_data.size,
      },
    };

    texture_view_layout_entry = {
      .chain = {
        .sType = (WGPUSType)WGPUSType_BindGroupLayoutEntryExtras,
        .next = nullptr,
      },
      .count = 2,
    };

    binding_layout[1] = {
      .nextInChain = &texture_view_layout_entry.chain,
      .binding = 1,
      .visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment,
      .texture = {
        .sampleType = WGPUTextureSampleType_Float,
        .viewDimension = WGPUTextureViewDimension_2D,
      },
    };

    binding_layout[2] = {
      .binding = 2,
      .visibility = WGPUShaderStage_Fragment,
      .sampler = {
        .type = WGPUSamplerBindingType_Filtering,
      },
    };

    WGPUBindGroupLayoutDescriptor bindGroupLayoutDesc = {
        .entryCount = (uint32_t)binding_layout.size(),
        .entries = binding_layout.data(),
    };

    bindGroupLayout = wgpuDeviceCreateBindGroupLayout(m_context.device, &bindGroupLayoutDesc);
    assert(bindGroupLayout != nullptr);

    std::array<WGPUTextureView, 2> texture_views = {
        texture.view,
        texture2.view,
    };

    binding[0] = {
        .binding = 0,
        .buffer = uniformBuffer,
        .offset = 0,
        .size = uniform_data.size,
    };

    texture_view_entry = {
      .chain = {
        .sType = (WGPUSType)WGPUSType_BindGroupEntryExtras,
        .next = nullptr,
      },
      .textureViews = texture_views.data(),
      .textureViewCount = 2,
    };

    binding[1] = {
        .nextInChain = &texture_view_entry.chain,
        .binding = 1,
    };

    binding[2] = {
        .binding = 2,
        .sampler = sampler,
    };

    WGPUBindGroupDescriptor bindGroupDesc = {
        .layout = bindGroupLayout,
        .entryCount = (uint32_t)binding.size(),
        .entries = binding.data(),
    };

    bindGroup = wgpuDeviceCreateBindGroup(m_context.device, &bindGroupDesc);
    assert(bindGroup != nullptr);
  }

  // Pipeline
  {
    // Uniforms layout
    WGPUPipelineLayoutDescriptor pipelineLayoutDesc{};
    pipelineLayoutDesc.bindGroupLayoutCount = 1;
    pipelineLayoutDesc.bindGroupLayouts = &bindGroupLayout;
    pipelineLayout = wgpuDeviceCreatePipelineLayout(m_context.device, &pipelineLayoutDesc);

    // Vertex fetch
    vertex_size = 6 * sizeof(float) + sizeof(uint32_t);
    std::array<WGPUVertexAttribute, 4> vertexAttribs = {
        WGPUVertexAttribute{
            .shaderLocation = 0,
            .format = WGPUVertexFormat_Float32x3,
            .offset = 0,
        },
        WGPUVertexAttribute{
            .shaderLocation = 1,
            .format = WGPUVertexFormat_Float32x2,
            .offset = 3 * sizeof(float),
        },
        WGPUVertexAttribute{
            .shaderLocation = 2,
            .format = WGPUVertexFormat_Float32,
            .offset = 5 * sizeof(float),
        },
        WGPUVertexAttribute{
            .shaderLocation = 3,
            .format = WGPUVertexFormat_Unorm8x4,
            .offset = 6 * sizeof(float),
        },
    };

    WGPUVertexBufferLayout vertexBufferLayout = {
        .attributeCount = vertexAttribs.size(),
        .attributes = vertexAttribs.data(),
        .arrayStride = vertex_size,
        .stepMode = WGPUVertexStepMode_Vertex,
    };

    // Blend state
    WGPUBlendState blendState = {
      .color = {
        .srcFactor = WGPUBlendFactor_SrcAlpha,
        .dstFactor = WGPUBlendFactor_OneMinusSrcAlpha,
        .operation = WGPUBlendOperation_Add,
      },
      .alpha = {
        .srcFactor = WGPUBlendFactor_Zero,
        .dstFactor = WGPUBlendFactor_One,
        .operation = WGPUBlendOperation_Add,
      },
    };

    WGPUColorTargetState colorTarget = {
        .format = m_context.surface_format,
        .blend = &blendState,
        .writeMask = WGPUColorWriteMask_All,
    };

    // Fragment state
    WGPUFragmentState fragmentState = {
        .module = shader.module,
        .entryPoint = "fs_main",
        .targetCount = 1,
        .targets = &colorTarget,
    };

    stencil_state = utils::default_depth_stencil_state();
    stencil_state.depthCompare = WGPUCompareFunction_Less;
    stencil_state.depthWriteEnabled = true;
    stencil_state.format = WGPUTextureFormat_Depth24Plus;
    stencil_state.stencilReadMask = 0;
    stencil_state.stencilWriteMask = 0;

    WGPURenderPipelineDescriptor pipelineDesc = {
      .layout = pipelineLayout,

      .vertex = {
        .bufferCount = 1,
        .buffers = &vertexBufferLayout,
        .module = shader.module,
        .entryPoint = "vs_main",
      },

      .primitive = {
        .topology = WGPUPrimitiveTopology_TriangleList,
        .frontFace = WGPUFrontFace_CCW,
        .cullMode = WGPUCullMode_None,
      },

      .fragment = &fragmentState,
      .depthStencil = &stencil_state,
      .multisample.count = 1,
    };

    pipeline = wgpuDeviceCreateRenderPipeline(m_context.device, &pipelineDesc);
    assert(pipeline != nullptr);
  }

  m_has_loaded = true;
}

void WorldPipeline::render(const WGPURenderPassEncoder render_pass, const Camera& camera)
{
  wgpuQueueWriteBuffer(m_context.queue,
                       uniformBuffer,
                       uniform_data.projection_matrix_offset,
                       &camera.projection_matrix,
                       uniform_data.projection_matrix_size);
  wgpuQueueWriteBuffer(m_context.queue,
                       uniformBuffer,
                       uniform_data.view_matrix_offset,
                       &camera.view_matrix,
                       uniform_data.view_matrix_size);

  wgpuRenderPassEncoderSetPipeline(render_pass, pipeline);
  wgpuRenderPassEncoderSetVertexBuffer(render_pass, 0, mesh.buffer, 0, mesh.size);
  wgpuRenderPassEncoderSetBindGroup(render_pass, 0, bindGroup, 0, nullptr);
  wgpuRenderPassEncoderDraw(render_pass, mesh.count, 1, 0, 0);
}

void WorldPipeline::sprite(Sprite* sprite, const double x, const double y, const double z) {}

void WorldPipeline::quad(const Quad* quad, const double x, const double y, const double z) {}

void WorldPipeline::text(Text& text, const double x, const double y, const double z) {}
}  // namespace dl::v2
