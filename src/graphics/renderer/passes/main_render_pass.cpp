#include "./main_render_pass.hpp"

#include <spdlog/spdlog.h>

#include "core/game_context.hpp"
#include "graphics/camera.hpp"
#include "graphics/display.hpp"
#include "graphics/renderer/utils.hpp"
#include "graphics/renderer/wgpu_context.hpp"

namespace dl
{
MainRenderPass::MainRenderPass(GameContext& game_context)
    : m_game_context(game_context), m_context(m_game_context.display->wgpu_context)
{
}

MainRenderPass::~MainRenderPass()
{
  if (!m_has_loaded)
  {
    return;
  }

  // Release batch resources
  wgpuBufferDestroy(batch.pipeline.uniform_buffer);
  wgpuBufferRelease(batch.pipeline.uniform_buffer);

  for (auto bind_group : batch.pipeline.bind_groups)
  {
    wgpuBindGroupRelease(bind_group);
  }
  for (auto bind_group_layout : batch.pipeline.bind_group_layouts)
  {
    wgpuBindGroupLayoutRelease(bind_group_layout);
  }

  wgpuSamplerRelease(batch.pipeline.sampler);
  wgpuPipelineLayoutRelease(batch.pipeline.layout);
  wgpuRenderPipelineRelease(batch.pipeline.pipeline);
}

void MainRenderPass::load(const Shader& shader, const WGPUTextureView depth_texture_view)
{
  batch.load();

  auto& pipeline = batch.pipeline;

  // Create sampler
  WGPUSamplerDescriptor sampler_descriptor = {
      .label = "Batch Sampler",
      .addressModeU = WGPUAddressMode_ClampToEdge,
      .addressModeV = WGPUAddressMode_ClampToEdge,
      .addressModeW = WGPUAddressMode_ClampToEdge,
      .magFilter = WGPUFilterMode_Nearest,
      .minFilter = WGPUFilterMode_Nearest,
      .mipmapFilter = WGPUMipmapFilterMode_Nearest,
      .lodMinClamp = 0.0f,
      .lodMaxClamp = 1.0f,
      .compare = WGPUCompareFunction_Undefined,
      .maxAnisotropy = 1,
  };

  pipeline.sampler = wgpuDeviceCreateSampler(m_context.device, &sampler_descriptor);
  assert(pipeline.sampler != nullptr);

  // Uniforms
  WGPUBufferDescriptor uniform_buffer_descriptor = {
      .label = "Batch Uniform Buffer",
      .size = pipeline.uniform_data.size,
      .usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform,
      .mappedAtCreation = false,
  };

  pipeline.uniform_buffer = wgpuDeviceCreateBuffer(m_context.device, &uniform_buffer_descriptor);
  assert(pipeline.uniform_buffer != nullptr);

  const auto identity_matrix = glm::mat4(1.0f);

  wgpuQueueWriteBuffer(m_context.queue,
                       pipeline.uniform_buffer,
                       pipeline.uniform_data.projection_matrix_offset,
                       &identity_matrix,
                       pipeline.uniform_data.projection_matrix_size);
  wgpuQueueWriteBuffer(m_context.queue,
                       pipeline.uniform_buffer,
                       pipeline.uniform_data.view_matrix_offset,
                       &identity_matrix,
                       pipeline.uniform_data.view_matrix_size);

  std::array<WGPUBindGroupLayoutEntry, 2> bind_group_layout_entries{};
  bind_group_layout_entries[BIND_GROUP_UNIFORMS] = {
    .binding = 0,
    .visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment,
    .buffer = {
      .type = WGPUBufferBindingType_Uniform,
      .minBindingSize = pipeline.uniform_data.size,
    },
  };

  bind_group_layout_entries[BIND_GROUP_TEXTURES] = {
    .binding = 1,
    .visibility = WGPUShaderStage_Fragment,
    .sampler = {
      .type = WGPUSamplerBindingType_Filtering,
    },
  };

  WGPUBindGroupLayoutDescriptor bind_group_layout_descriptor = {
      .entryCount = static_cast<uint32_t>(bind_group_layout_entries.size()),
      .entries = bind_group_layout_entries.data(),
  };

  pipeline.bind_group_layouts[BIND_GROUP_UNIFORMS]
      = wgpuDeviceCreateBindGroupLayout(m_context.device, &bind_group_layout_descriptor);
  assert(pipeline.bind_group_layouts[BIND_GROUP_UNIFORMS] != nullptr);

  std::array<WGPUBindGroupEntry, 2> bind_group_entries{};
  bind_group_entries[BIND_GROUP_UNIFORMS] = {
      .binding = 0,
      .buffer = pipeline.uniform_buffer,
      .offset = 0,
      .size = pipeline.uniform_data.size,
  };

  bind_group_entries[BIND_GROUP_TEXTURES] = {
      .binding = 1,
      .sampler = pipeline.sampler,
  };

  WGPUBindGroupDescriptor bind_group_descriptor = {
      .layout = pipeline.bind_group_layouts[BIND_GROUP_UNIFORMS],
      .entryCount = static_cast<uint32_t>(bind_group_entries.size()),
      .entries = bind_group_entries.data(),
  };

  pipeline.bind_groups[BIND_GROUP_UNIFORMS] = wgpuDeviceCreateBindGroup(m_context.device, &bind_group_descriptor);
  assert(pipeline.bind_groups[BIND_GROUP_UNIFORMS] != nullptr);

  m_update_texture_bind_group();

  // Create pipeline
  // Uniforms layout
  WGPUPipelineLayoutDescriptor pipeline_layout_descriptor{};
  pipeline_layout_descriptor.bindGroupLayoutCount = pipeline.bind_group_layouts.size();
  pipeline_layout_descriptor.bindGroupLayouts = pipeline.bind_group_layouts.data();
  pipeline.layout = wgpuDeviceCreatePipelineLayout(m_context.device, &pipeline_layout_descriptor);

  // Vertex fetch
  std::array<WGPUVertexAttribute, 4> vertex_attributes = {
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

  WGPUVertexBufferLayout batch_datum_layout = {
      .attributeCount = vertex_attributes.size(),
      .attributes = vertex_attributes.data(),
      .arrayStride = sizeof(VertexData),
      .stepMode = WGPUVertexStepMode_Vertex,
  };

  // Blend state
  WGPUBlendState blend_state = {
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

  WGPUColorTargetState color_target = {
      .format = m_context.surface_format,
      .blend = &blend_state,
      .writeMask = WGPUColorWriteMask_All,
  };

  // Fragment state
  WGPUFragmentState fragment_state = {
      .module = shader.module,
      .entryPoint = "fs_main",
      .targetCount = 1,
      .targets = &color_target,
  };

  WGPURenderPipelineDescriptor pipeline_descriptor = {
    .layout = pipeline.layout,

    .vertex = {
      .bufferCount = 1,
      .buffers = &batch_datum_layout,
      .module = shader.module,
      .entryPoint = "vs_main",
    },

    .primitive = {
      .topology = WGPUPrimitiveTopology_TriangleList,
      .frontFace = WGPUFrontFace_CCW,
      .cullMode = WGPUCullMode_None,
    },

    .fragment = &fragment_state,
    .depthStencil = nullptr,
    .multisample.count = 1,
  };

  WGPUDepthStencilState stencil_state;
  stencil_state = utils::default_depth_stencil_state();
  stencil_state.depthCompare = WGPUCompareFunction_Less;
  stencil_state.depthWriteEnabled = true;
  stencil_state.format = WGPUTextureFormat_Depth24Plus;
  stencil_state.stencilReadMask = 0;
  stencil_state.stencilWriteMask = 0;

  pipeline_descriptor.depthStencil = &stencil_state;

  pipeline.pipeline = wgpuDeviceCreateRenderPipeline(m_context.device, &pipeline_descriptor);
  assert(pipeline.pipeline != nullptr);

  // Set up render pass
  render_pass_color_attachment = {
      .resolveTarget = nullptr,
      .loadOp = WGPULoadOp_Clear,
      .storeOp = WGPUStoreOp_Store,
      .clearValue = clear_color,
  };

  depth_stencil_attachment = {
      .view = depth_texture_view,
      .depthClearValue = 1.0f,
      .depthLoadOp = WGPULoadOp_Clear,
      .depthStoreOp = WGPUStoreOp_Store,
      .depthReadOnly = false,
      .stencilClearValue = 0,
      .stencilLoadOp = WGPULoadOp_Clear,
      .stencilStoreOp = WGPUStoreOp_Store,
      .stencilReadOnly = true,
  };

  render_pass_descriptor = {
      .timestampWrites = nullptr,
      .depthStencilAttachment = &depth_stencil_attachment,
      .colorAttachmentCount = 1,
  };

  m_has_loaded = true;
}

void MainRenderPass::resize(const WGPUTextureView depth_texture_view)
{
  depth_stencil_attachment.view = depth_texture_view;
}

void MainRenderPass::render(WGPUTextureView target_view, WGPUCommandEncoder encoder, const Camera& camera)
{
  render_pass_color_attachment.clearValue = clear_color;
  render_pass_color_attachment.view = target_view;
  render_pass_descriptor.colorAttachments = &render_pass_color_attachment;
  WGPURenderPassEncoder render_pass = wgpuCommandEncoderBeginRenderPass(encoder, &render_pass_descriptor);

  if (!batch.empty())
  {
    auto& pipeline = batch.pipeline;

    if (batch.should_update_texture_bind_group)
    {
      m_update_texture_bind_group();
    }

    // Set up uniforms
    wgpuQueueWriteBuffer(m_context.queue,
                         pipeline.uniform_buffer,
                         pipeline.uniform_data.projection_matrix_offset,
                         &camera.projection_matrix,
                         pipeline.uniform_data.projection_matrix_size);

    wgpuQueueWriteBuffer(m_context.queue,
                         pipeline.uniform_buffer,
                         pipeline.uniform_data.view_matrix_offset,
                         &camera.view_matrix,
                         pipeline.uniform_data.view_matrix_size);

    // Set up pipeline
    wgpuRenderPassEncoderSetPipeline(render_pass, pipeline.pipeline);

    // Set up bind groups
    wgpuRenderPassEncoderSetBindGroup(render_pass, 0, pipeline.bind_groups[BIND_GROUP_UNIFORMS], 0, nullptr);
    wgpuRenderPassEncoderSetBindGroup(render_pass, 1, pipeline.bind_groups[BIND_GROUP_TEXTURES], 0, nullptr);

    for (auto& batch_datum : batch.batch_data)
    {
      if (batch_datum.index_buffer_count == 0)
      {
        continue;
      }

      // Write vertex data to buffer
      batch_datum.update(m_context.queue);
      wgpuRenderPassEncoderSetVertexBuffer(
          render_pass, 0, batch_datum.vertex_buffer, 0, batch_datum.vertex_buffer_size);
      wgpuRenderPassEncoderSetIndexBuffer(
          render_pass, batch_datum.index_buffer, WGPUIndexFormat_Uint32, 0, batch_datum.index_buffer_size);

      // Set scissor if exists
      if (batch_datum.has_scissor())
      {
        const auto& scissor = batch_datum.scissor;
        wgpuRenderPassEncoderSetScissorRect(render_pass, scissor.x, scissor.y, scissor.z, scissor.w);
      }

      // Draw
      wgpuRenderPassEncoderDrawIndexed(render_pass, batch_datum.index_buffer_count, 1, 0, 0, 0);

      // Reset buffer for next frame
      batch_datum.reset();
    }
  }

  wgpuRenderPassEncoderEnd(render_pass);
  wgpuRenderPassEncoderRelease(render_pass);
}

void MainRenderPass::m_update_texture_bind_group()
{
  auto& pipeline = batch.pipeline;
  WGPUBindGroupLayoutEntryExtras texture_view_layout_entry = {
    .chain = {
      .sType = (WGPUSType)WGPUSType_BindGroupLayoutEntryExtras,
      .next = nullptr,
    },
    .count = static_cast<uint32_t>(batch.texture_views.size()),
  };

  WGPUBindGroupLayoutEntry texture_binding_layout = {
    .nextInChain = &texture_view_layout_entry.chain,
    .binding = 0,
    .visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment,
    .texture = {
      .sampleType = WGPUTextureSampleType_Float,
      .viewDimension = WGPUTextureViewDimension_2D,
    },
  };

  WGPUBindGroupLayoutDescriptor texture_bind_group_layout_descriptor = {
      .entryCount = 1,
      .entries = &texture_binding_layout,
  };

  pipeline.bind_group_layouts[BIND_GROUP_TEXTURES]
      = wgpuDeviceCreateBindGroupLayout(m_context.device, &texture_bind_group_layout_descriptor);
  assert(pipeline.bind_group_layouts[BIND_GROUP_TEXTURES] != nullptr);

  WGPUBindGroupEntryExtras texture_view_entry = {
    .chain = {
      .sType = (WGPUSType)WGPUSType_BindGroupEntryExtras,
      .next = nullptr,
    },
    .textureViews = batch.texture_views.data(),
    .textureViewCount = static_cast<uint32_t>(batch.texture_views.size()),
  };

  WGPUBindGroupEntry texture_binding = {
      .nextInChain = &texture_view_entry.chain,
      .binding = 0,
  };

  WGPUBindGroupDescriptor texture_bind_group_descriptor = {
      .layout = pipeline.bind_group_layouts[BIND_GROUP_TEXTURES],
      .entryCount = 1,
      .entries = &texture_binding,
  };
  pipeline.bind_groups[BIND_GROUP_TEXTURES]
      = wgpuDeviceCreateBindGroup(m_context.device, &texture_bind_group_descriptor);
  assert(pipeline.bind_groups[BIND_GROUP_TEXTURES] != nullptr);

  batch.should_update_texture_bind_group = false;
}

}  // namespace dl
