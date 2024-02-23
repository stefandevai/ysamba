#include "./world.hpp"

#include <spdlog/spdlog.h>

#include "core/display.hpp"
#include "core/game_context.hpp"
#include "graphics/camera.hpp"
#include "graphics/color.hpp"
#include "graphics/frame_angle.hpp"
#include "graphics/multi_sprite.hpp"
#include "graphics/nine_patch.hpp"
#include "graphics/quad.hpp"
#include "graphics/renderer/utils.hpp"
#include "graphics/sprite.hpp"
#include "graphics/text.hpp"
#include "graphics/tile_render_data.hpp"

namespace dl
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
  // Add main vertex buffer
  m_vertex_buffers.emplace_back(m_context.device, MAIN_BATCH_VERTEX_COUNT);
  m_current_vb = &m_vertex_buffers[0];

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
        .arrayStride = sizeof(VertexData),
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
  assert(m_current_vb != nullptr);

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

  // Set up pipeline
  wgpuRenderPassEncoderSetPipeline(render_pass, pipeline);

  // Set up bind groups
  wgpuRenderPassEncoderSetBindGroup(render_pass, 0, bindGroup, 0, nullptr);
  wgpuRenderPassEncoderSetBindGroup(render_pass, 1, texture_bind_group, 0, nullptr);

  for (auto& vertex_buffer : m_vertex_buffers)
  {
    if (vertex_buffer.index == 0)
    {
      continue;
    }

    // Write vertex data to buffer
    vertex_buffer.update(m_context.queue);
    wgpuRenderPassEncoderSetVertexBuffer(render_pass, 0, vertex_buffer.buffer, 0, vertex_buffer.size);

    // Set scissor if exists
    if (vertex_buffer.has_scissor())
    {
      const auto& scissor = vertex_buffer.scissor;
      wgpuRenderPassEncoderSetScissorRect(render_pass, scissor.x, scissor.y, scissor.z, scissor.w);
    }

    // Draw
    wgpuRenderPassEncoderDraw(render_pass, vertex_buffer.index, 1, 0, 0);

    // Reset buffer for next frame
    vertex_buffer.reset();
  }
}

void WorldPipeline::clear_textures() { m_texture_slot_index = 0; }

void WorldPipeline::sprite(Sprite* sprite, const double x, const double y, const double z)
{
  assert(m_current_vb != nullptr);

  if (sprite->texture == nullptr)
  {
    sprite->texture = m_game_context.asset_manager->get<Texture>("ysamba-typography"_hs, m_context.device);
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
    m_current_vb->emplace(glm::vec3{x, y, z}, texture_coordinates[0], texture_index, color);
  }
  else
  {
    m_current_vb->emplace(glm::vec3{x, y + size.y, z + size.y}, texture_coordinates[0], texture_index, color);
  }

  // Top right vertex
  if (sprite->frame_angle == FrameAngle::Parallel)
  {
    m_current_vb->emplace(glm::vec3{x + size.x, y, z}, texture_coordinates[1], texture_index, color);
  }
  else
  {
    m_current_vb->emplace(glm::vec3{x + size.x, y + size.y, z + size.y}, texture_coordinates[1], texture_index, color);
  }

  // Bottom left vertex
  m_current_vb->emplace(glm::vec3{x, y + size.y, z}, texture_coordinates[3], texture_index, color);

  // Top right vertex
  if (sprite->frame_angle == FrameAngle::Parallel)
  {
    m_current_vb->emplace(glm::vec3{x + size.x, y, z}, texture_coordinates[1], texture_index, color);
  }
  else
  {
    m_current_vb->emplace(glm::vec3{x + size.x, y + size.y, z + size.y}, texture_coordinates[1], texture_index, color);
  }

  // Bottom left vertex
  m_current_vb->emplace(glm::vec3{x, y + size.y, z}, texture_coordinates[3], texture_index, color);

  // Bottom right vertex
  m_current_vb->emplace(glm::vec3{x + size.x, y + size.y, z}, texture_coordinates[2], texture_index, color);
}

void WorldPipeline::multi_sprite(MultiSprite* sprite, const double x, const double y, const double z)
{
  assert(m_current_vb != nullptr);

  // Load texture if it has not been loaded
  if (sprite->texture == nullptr)
  {
    sprite->texture = m_game_context.asset_manager->get<Texture>(sprite->resource_id, m_context.device);
  }

  unsigned int color = sprite->color.int_color;

  assert(sprite->texture != nullptr);

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

  const auto& size = sprite->get_size();
  const std::array<glm::vec2, 4> texture_coordinates = sprite->get_texture_coordinates();
  const int frame_width = sprite->texture->get_frame_width() * (size.x);
  const int frame_height = sprite->texture->get_frame_height() * (size.y);

  // Top left vertex
  // Change according to the angle that the quad will be rendered
  if (sprite->frame_angle == FrameAngle::Parallel)
  {
    m_current_vb->emplace(glm::vec3{x, y, z}, texture_coordinates[0], texture_index, color);
  }
  else
  {
    m_current_vb->emplace(
        glm::vec3{x, y + frame_height, z + frame_height}, texture_coordinates[0], texture_index, color);
  }

  // Top right vertex
  if (sprite->frame_angle == FrameAngle::Parallel)
  {
    m_current_vb->emplace(glm::vec3{x + frame_width, y, z}, texture_coordinates[1], texture_index, color);
  }
  else
  {
    m_current_vb->emplace(
        glm::vec3{x + frame_width, y + frame_height, z + frame_height}, texture_coordinates[1], texture_index, color);
  }

  // Bottom left vertex
  m_current_vb->emplace(glm::vec3{x, y + frame_height, z}, texture_coordinates[3], texture_index, color);

  // Top right vertex
  if (sprite->frame_angle == FrameAngle::Parallel)
  {
    m_current_vb->emplace(glm::vec3{x + frame_width, y, z}, texture_coordinates[1], texture_index, color);
  }
  else
  {
    m_current_vb->emplace(
        glm::vec3{x + frame_width, y + frame_height, z + frame_height}, texture_coordinates[1], texture_index, color);
  }

  // Bottom left vertex
  m_current_vb->emplace(glm::vec3{x, y + frame_height, z}, texture_coordinates[3], texture_index, color);

  // Bottom right vertex
  m_current_vb->emplace(glm::vec3{x + frame_width, y + frame_height, z}, texture_coordinates[2], texture_index, color);
}

void WorldPipeline::tile(const TileRenderData& tile, const double x, const double y, const double z)
{
  assert(m_current_vb != nullptr);

  const auto& size = tile.size;
  const auto& uv_coordinates = tile.uv_coordinates;
  const uint32_t color = 0xFFFFFFFF;

  assert(tile.texture != nullptr);
  assert(size.x != 0);
  assert(size.y != 0);

  float texture_index = 0.00f;
  const auto upper_bound = m_texture_views.begin() + m_texture_slot_index;
  const auto it = std::find(m_texture_views.begin(), upper_bound, tile.texture->view);
  if (it >= upper_bound)
  {
    texture_index = static_cast<float>(m_texture_slot_index);
    m_texture_views[m_texture_slot_index] = tile.texture->view;
    ++m_texture_slot_index;
    m_should_update_texture_bind_group = true;
  }
  else
  {
    texture_index = it - m_texture_views.begin();
  }

  // Top left vertex
  if (tile.frame_data->angle == FrameAngle::Parallel)
  {
    m_current_vb->emplace(glm::vec3{x, y, z}, uv_coordinates[0], texture_index, color);
  }
  else
  {
    m_current_vb->emplace(glm::vec3{x, y + size.y, z + size.y}, uv_coordinates[0], texture_index, color);
  }

  // Top right vertex
  if (tile.frame_data->angle == FrameAngle::Parallel)
  {
    m_current_vb->emplace(glm::vec3{x + size.x, y, z}, uv_coordinates[1], texture_index, color);
  }
  else
  {
    m_current_vb->emplace(glm::vec3{x + size.x, y + size.y, z + size.y}, uv_coordinates[1], texture_index, color);
  }

  // Bottom left vertex
  m_current_vb->emplace(glm::vec3{x, y + size.y, z}, uv_coordinates[3], texture_index, color);

  // Top right vertex
  if (tile.frame_data->angle == FrameAngle::Parallel)
  {
    m_current_vb->emplace(glm::vec3{x + size.x, y, z}, uv_coordinates[1], texture_index, color);
  }
  else
  {
    m_current_vb->emplace(glm::vec3{x + size.x, y + size.y, z + size.y}, uv_coordinates[1], texture_index, color);
  }

  // Bottom left vertex
  m_current_vb->emplace(glm::vec3{x, y + size.y, z}, uv_coordinates[3], texture_index, color);

  // Bottom right vertex
  m_current_vb->emplace(glm::vec3{x + size.x, y + size.y, z}, uv_coordinates[2], texture_index, color);
}

void WorldPipeline::quad(const Quad* quad, const double x, const double y, const double z)
{
  assert(m_current_vb != nullptr);

  uint32_t color = quad->color.int_color;

  if (quad->color.opacity_factor < 1.0)
  {
    const auto& quad_color = quad->color.rgba_color;
    color = Color::rgba_to_int(
        quad_color.r, quad_color.g, quad_color.b, static_cast<uint8_t>(quad_color.a * quad->color.opacity_factor));
  }

  // Top left vertex
  m_current_vb->emplace(glm::vec3{x, y, z}, glm::vec2{0}, -1.0f, color);

  // Top right vertex
  m_current_vb->emplace(glm::vec3{x + quad->w, y, z}, glm::vec2{0}, -1.0f, color);

  // Bottom left vertex
  m_current_vb->emplace(glm::vec3{x, y + quad->h, z}, glm::vec2{0}, -1.0f, color);

  // Top right vertex
  m_current_vb->emplace(glm::vec3{x + quad->w, y, z}, glm::vec2{0}, -1.0f, color);

  // Bottom left vertex
  m_current_vb->emplace(glm::vec3{x, y + quad->h, z}, glm::vec2{0}, -1.0f, color);

  // Bottom right vertex
  m_current_vb->emplace(glm::vec3{x + quad->w, y + quad->h, z}, glm::vec2{0}, -1.0f, color);
}

void WorldPipeline::text(Text& text, const double x, const double y, const double z)
{
  assert(m_current_vb != nullptr);

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

    if (character.sprite->color.opacity_factor != text.color.opacity_factor)
    {
      character.sprite->color.opacity_factor = text.color.opacity_factor;
    }

    sprite(character.sprite.get(), character.x + x, character.y + y, z);
  }
}

void WorldPipeline::m_update_texture_bind_group()
{
  assert(m_current_vb != nullptr);

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

void WorldPipeline::nine_patch(NinePatch& nine_patch, const double x, const double y, const double z)
{
  assert(m_current_vb != nullptr);

  if (nine_patch.texture == nullptr)
  {
    nine_patch.texture = m_game_context.asset_manager->get<Texture>(nine_patch.resource_id, m_context.device);
  }

  if (nine_patch.dirty)
  {
    nine_patch.generate_patches();
  }
  if (nine_patch.color.opacity_factor != nine_patch.center_patch.color.opacity_factor)
  {
    nine_patch.center_patch.color.opacity_factor = nine_patch.color.opacity_factor;

    for (auto& patch : nine_patch.border_patches)
    {
      patch.color.opacity_factor = nine_patch.color.opacity_factor;
    }
  }

  // Top left patch
  sprite(&nine_patch.border_patches[0], x, y, z);
  // Top center patch
  sprite(&nine_patch.border_patches[1], x + nine_patch.border, y, z);
  // Top right patch
  sprite(&nine_patch.border_patches[2], x + nine_patch.size.x - nine_patch.border, y, z);
  // Center right patch
  sprite(&nine_patch.border_patches[3], x + nine_patch.size.x - nine_patch.border, y + nine_patch.border, z);
  // Bottom right patch
  sprite(&nine_patch.border_patches[4],
         x + nine_patch.size.x - nine_patch.border,
         y + nine_patch.size.y - nine_patch.border,
         z);
  // Bottom center patch
  sprite(&nine_patch.border_patches[5], x + nine_patch.border, y + nine_patch.size.y - nine_patch.border, z);
  // Bottom left patch
  sprite(&nine_patch.border_patches[6], x, y + nine_patch.size.y - nine_patch.border, z);
  // Center left patch
  sprite(&nine_patch.border_patches[7], x, y + nine_patch.border, z);

  // Center patch
  sprite(&nine_patch.center_patch, x + nine_patch.border, y + nine_patch.border, z);
}

void WorldPipeline::push_scissor(Vector4i scissor)
{
  // Try to reuse a vertex buffer
  if (m_vertex_buffers.size() > 1)
  {
    for (auto& vertex_buffer : m_vertex_buffers)
    {
      if (vertex_buffer.index == 0)
      {
        vertex_buffer.scissor = std::move(scissor);
        m_current_vb = &vertex_buffer;
        return;
      }
    }
  }

  // Create a new buffer
  VertexBuffer<VertexData> vertex_buffer{m_context.device, SECONDARY_BATCH_VERTEX_COUNT};
  vertex_buffer.scissor = std::move(scissor);
  m_vertex_buffers.push_back(std::move(vertex_buffer));
  m_current_vb = &m_vertex_buffers.back();
}

void WorldPipeline::pop_scissor() { m_current_vb = &m_vertex_buffers[0]; }
}  // namespace dl
