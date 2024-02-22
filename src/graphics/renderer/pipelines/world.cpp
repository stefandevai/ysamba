#include "./world.hpp"

#include <spdlog/spdlog.h>

#include "core/display.hpp"
#include "core/game_context.hpp"
#include "graphics/camera.hpp"
#include "graphics/color.hpp"
#include "graphics/frame_angle.hpp"
#include "graphics/quad.hpp"
#include "graphics/renderer/utils.hpp"
#include "graphics/renderer/wgpu_context.hpp"
#include "graphics/sprite.hpp"
#include "graphics/text.hpp"

namespace dl::v2
{
WorldPipeline::WorldPipeline(GameContext& game_context)
    : m_game_context(game_context),
      m_context(game_context.display->wgpu_context),
      m_dummy_texture(Texture::dummy(m_context.device))
{
}

WorldPipeline::~WorldPipeline()
{
  if (m_has_loaded)
  {
    wgpuBufferDestroy(uniformBuffer);
    wgpuBufferRelease(uniformBuffer);
    wgpuBindGroupRelease(bindGroup);
    wgpuBindGroupLayoutRelease(bindGroupLayouts[0]);
    wgpuBindGroupRelease(texture_bind_group);
    wgpuBindGroupLayoutRelease(bindGroupLayouts[1]);
    wgpuSamplerRelease(sampler);
    wgpuPipelineLayoutRelease(pipelineLayout);
    wgpuRenderPipelineRelease(pipeline);
  }
}

void WorldPipeline::load(const Shader& shader)
{
  m_vertices.resize(m_indices_size);

  // Mesh
  mesh.load(m_context.device);

  // Textures
  // TODO: Replace dummy textures with a more robust and flexible system
  // when https://github.com/gfx-rs/wgpu/issues/3692 is available
  for (uint32_t i = 0; i < TEXTURE_SLOTS; ++i)
  {
    m_texture_views[i] = m_dummy_texture.view;
  }

  m_texture_slot_index = 0;

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

    binding_layout[1] = {
      .binding = 1,
      .visibility = WGPUShaderStage_Fragment,
      .sampler = {
        .type = WGPUSamplerBindingType_Filtering,
      },
    };

    WGPUBindGroupLayoutDescriptor bindGroupLayoutDesc = {
        .entryCount = (uint32_t)binding_layout.size(),
        .entries = binding_layout.data(),
    };

    bindGroupLayouts[0] = wgpuDeviceCreateBindGroupLayout(m_context.device, &bindGroupLayoutDesc);
    assert(bindGroupLayouts[0] != nullptr);

    binding[0] = {
        .binding = 0,
        .buffer = uniformBuffer,
        .offset = 0,
        .size = uniform_data.size,
    };

    binding[1] = {
        .binding = 1,
        .sampler = sampler,
    };

    WGPUBindGroupDescriptor bindGroupDesc = {
        .layout = bindGroupLayouts[0],
        .entryCount = (uint32_t)binding.size(),
        .entries = binding.data(),
    };

    bindGroup = wgpuDeviceCreateBindGroup(m_context.device, &bindGroupDesc);
    assert(bindGroup != nullptr);

    m_update_texture_bind_group();
  }

  // Pipeline
  {
    // Uniforms layout
    WGPUPipelineLayoutDescriptor pipelineLayoutDesc{};
    pipelineLayoutDesc.bindGroupLayoutCount = bindGroupLayouts.size();
    pipelineLayoutDesc.bindGroupLayouts = bindGroupLayouts.data();
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

  // Initialize vertex buffer
  {
    WGPUBufferDescriptor buffer_descriptor = {
        .size = m_buffer_size,
        .usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex,
        .mappedAtCreation = false,
    };
    m_vertex_buffer = wgpuDeviceCreateBuffer(m_context.device, &buffer_descriptor);
  }

  m_has_loaded = true;
}

void WorldPipeline::render(const WGPURenderPassEncoder render_pass, const Camera& camera)
{
  if (m_should_update_texture_bind_group)
  {
    m_update_texture_bind_group();
  }

  // Set up uniforms
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

  // Write vertice data to buffer
  const auto size = m_vertices_index * sizeof(VertexData);
  const auto count = m_vertices_index;
  wgpuQueueWriteBuffer(m_context.queue, m_vertex_buffer, 0, m_vertices.data(), size);

  // Draw
  wgpuRenderPassEncoderSetPipeline(render_pass, pipeline);
  wgpuRenderPassEncoderSetVertexBuffer(render_pass, 0, m_vertex_buffer, 0, size);
  wgpuRenderPassEncoderSetBindGroup(render_pass, 0, bindGroup, 0, nullptr);
  wgpuRenderPassEncoderSetBindGroup(render_pass, 1, texture_bind_group, 0, nullptr);
  wgpuRenderPassEncoderDraw(render_pass, count, 1, 0, 0);

  // Reset vertex index for next frame
  index_count = 0;
  m_vertices_index = 0;
}

void WorldPipeline::sprite(Sprite* sprite, const double x, const double y, const double z)
{
  assert(index_count <= m_indices_size);
  if (sprite->texture == nullptr)
  {
    sprite->texture = m_game_context.asset_manager->get<v2::Texture>("ysamba-typography"_hs, m_context.device);
  }

  const glm::vec2& size = sprite->get_size();
  const auto& texture_coordinates = sprite->get_texture_coordinates();
  unsigned int color = sprite->color.int_color;

  assert(size.x != 0);
  assert(size.y != 0);

  if (sprite->color.opacity_factor < 1.0)
  {
    const auto& sprite_color = sprite->color.rgba_color;
    color = Color::rgba_to_int(sprite_color.r,
                               sprite_color.g,
                               sprite_color.b,
                               static_cast<uint8_t>(sprite_color.a * sprite->color.opacity_factor));
  }

  // Build vector of textures to bind when rendering
  // texture_index is the index in m_texture_views that will
  // be translated to a index in the shader.
  float texture_index = 0.00f;
  const auto upper_bound = m_texture_views.begin() + m_texture_slot_index;
  const auto it = std::find(m_texture_views.begin(), upper_bound, sprite->texture->view);
  if (it >= upper_bound)
  {
    texture_index = static_cast<float>(m_texture_slot_index);
    m_texture_views[m_texture_slot_index] = sprite->texture->view;
    ++m_texture_slot_index;
    m_should_update_texture_bind_group = true;
  }
  else
  {
    texture_index = it - m_texture_views.begin();
  }

  // Top left vertex
  if (sprite->frame_angle == FrameAngle::Parallel)
  {
    m_vertices[m_vertices_index++] = VertexData{glm::vec3{x, y, z}, texture_coordinates[0], texture_index, color};
  }
  else
  {
    m_vertices[m_vertices_index++]
        = VertexData{glm::vec3{x, y + size.y, z + size.y}, texture_coordinates[0], texture_index, color};
  }

  // Top right vertex
  if (sprite->frame_angle == FrameAngle::Parallel)
  {
    m_vertices[m_vertices_index++]
        = VertexData{glm::vec3{x + size.x, y, z}, texture_coordinates[1], texture_index, color};
  }
  else
  {
    m_vertices[m_vertices_index++]
        = VertexData{glm::vec3{x + size.x, y + size.y, z + size.y}, texture_coordinates[1], texture_index, color};
  }

  // Bottom left vertex
  m_vertices[m_vertices_index++]
      = VertexData{glm::vec3{x, y + size.y, z}, texture_coordinates[3], texture_index, color};

  // Top right vertex
  if (sprite->frame_angle == FrameAngle::Parallel)
  {
    m_vertices[m_vertices_index++]
        = VertexData{glm::vec3{x + size.x, y, z}, texture_coordinates[1], texture_index, color};
  }
  else
  {
    m_vertices[m_vertices_index++]
        = VertexData{glm::vec3{x + size.x, y + size.y, z + size.y}, texture_coordinates[1], texture_index, color};
  }

  // Bottom left vertex
  m_vertices[m_vertices_index++]
      = VertexData{glm::vec3{x, y + size.y, z}, texture_coordinates[3], texture_index, color};

  // Bottom right vertex
  m_vertices[m_vertices_index++]
      = VertexData{glm::vec3{x + size.x, y + size.y, z}, texture_coordinates[2], texture_index, color};

  // Each quad has 6 vertices, we have therefore to increment by 6 each time
  index_count += 6;
}

void WorldPipeline::quad(const Quad* quad, const double x, const double y, const double z) {}

void WorldPipeline::text(Text& text, const double x, const double y, const double z)
{
  if (!text.m_has_initialized)
  {
    text.m_font = m_game_context.asset_manager->get<Font>(text.typeface, m_context.device);
    assert(text.m_font != nullptr);
    text.initialize();
  }
  else if (!text.get_is_static())
  {
    text.update();
  }

  for (auto& character : text.characters)
  {
    // Character is a space
    if (character.sprite == nullptr)
    {
      continue;
    }
    /* if (character.sprite->color.int_color != text.color.int_color) */
    /* { */
    /*   character.sprite->color.set(text.color.int_color); */
    /* } */
    if (character.sprite->color.opacity_factor != text.color.opacity_factor)
    {
      character.sprite->color.opacity_factor = text.color.opacity_factor;
    }

    sprite(character.sprite.get(), character.x + x, character.y + y, z);
  }
}

void WorldPipeline::m_update_texture_bind_group()
{
  WGPUBindGroupLayoutEntryExtras texture_view_layout_entry = {
    .chain = {
      .sType = (WGPUSType)WGPUSType_BindGroupLayoutEntryExtras,
      .next = nullptr,
    },
    .count = (uint32_t)m_texture_views.size(),
  };

  texture_binding_layout = {
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

  bindGroupLayouts[1] = wgpuDeviceCreateBindGroupLayout(m_context.device, &texture_bind_group_layout_descriptor);
  assert(bindGroupLayouts[1] != nullptr);

  WGPUBindGroupEntryExtras texture_view_entry = {
    .chain = {
      .sType = (WGPUSType)WGPUSType_BindGroupEntryExtras,
      .next = nullptr,
    },
    .textureViews = m_texture_views.data(),
    .textureViewCount = (uint32_t)m_texture_views.size(),
  };

  texture_binding = {
      .nextInChain = &texture_view_entry.chain,
      .binding = 0,
  };

  WGPUBindGroupDescriptor texture_bind_group_descriptor = {
      .layout = bindGroupLayouts[1],
      .entryCount = 1,
      .entries = &texture_binding,
  };
  texture_bind_group = wgpuDeviceCreateBindGroup(m_context.device, &texture_bind_group_descriptor);
  assert(texture_bind_group != nullptr);

  m_should_update_texture_bind_group = false;
}
}  // namespace dl::v2
