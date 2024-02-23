#include "./batch.hpp"

#include <spdlog/spdlog.h>

#include "core/asset_manager.hpp"
#include "core/game_context.hpp"
#include "graphics/camera.hpp"
#include "graphics/color.hpp"
#include "graphics/display.hpp"
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
Batch::Batch(WGPUContext& context) : m_context(context), m_dummy_texture(Texture::dummy(m_context.device)) {}

void Batch::load()
{
  m_load_vertex_buffers();
  m_load_textures();
}

void Batch::m_load_vertex_buffers()
{
  // Add main vertex buffer
  vertex_buffers.emplace_back(m_context.device, MAIN_BATCH_VERTEX_COUNT);
  m_current_vb = &vertex_buffers[0];
}

void Batch::m_load_textures()
{
  // Textures
  // TODO: Replace dummy textures with a more robust and flexible system
  // when https://github.com/gfx-rs/wgpu/issues/3692 is available
  for (uint32_t i = 0; i < TEXTURE_SLOTS; ++i)
  {
    texture_views[i] = m_dummy_texture.view;
  }

  m_texture_slot_index = 0;
}

void Batch::clear_textures() { m_texture_slot_index = 0; }

void Batch::sprite(Sprite* sprite, const double x, const double y, const double z)
{
  assert(m_current_vb != nullptr);

  if (sprite->texture == nullptr)
  {
    sprite->texture = AssetManager::get<Texture>(sprite->resource_id, m_context.device);
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
  // texture_index is the index in texture_views that will
  // be translated to a index in the shader.
  float texture_index = 0.00f;
  const auto upper_bound = texture_views.begin() + m_texture_slot_index;
  const auto it = std::find(texture_views.begin(), upper_bound, sprite->texture->view);
  if (it >= upper_bound)
  {
    texture_index = static_cast<float>(m_texture_slot_index);
    texture_views[m_texture_slot_index] = sprite->texture->view;
    ++m_texture_slot_index;
    should_update_texture_bind_group = true;
  }
  else
  {
    texture_index = it - texture_views.begin();
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

void Batch::multi_sprite(MultiSprite* sprite, const double x, const double y, const double z)
{
  assert(m_current_vb != nullptr);

  // Load texture if it has not been loaded
  if (sprite->texture == nullptr)
  {
    sprite->texture = AssetManager::get<Texture>(sprite->resource_id, m_context.device);
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
  // texture_index is the index in texture_views that will
  // be translated to a index in the shader.
  float texture_index = 0.00f;
  const auto upper_bound = texture_views.begin() + m_texture_slot_index;
  const auto it = std::find(texture_views.begin(), upper_bound, sprite->texture->view);
  if (it >= upper_bound)
  {
    texture_index = static_cast<float>(m_texture_slot_index);
    texture_views[m_texture_slot_index] = sprite->texture->view;
    ++m_texture_slot_index;
    should_update_texture_bind_group = true;
  }
  else
  {
    texture_index = it - texture_views.begin();
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

void Batch::tile(const TileRenderData& tile, const double x, const double y, const double z)
{
  assert(m_current_vb != nullptr);

  const auto& size = tile.size;
  const auto& uv_coordinates = tile.uv_coordinates;
  const uint32_t color = 0xFFFFFFFF;

  assert(tile.texture != nullptr);
  assert(size.x != 0);
  assert(size.y != 0);

  float texture_index = 0.00f;
  const auto upper_bound = texture_views.begin() + m_texture_slot_index;
  const auto it = std::find(texture_views.begin(), upper_bound, tile.texture->view);
  if (it >= upper_bound)
  {
    texture_index = static_cast<float>(m_texture_slot_index);
    texture_views[m_texture_slot_index] = tile.texture->view;
    ++m_texture_slot_index;
    should_update_texture_bind_group = true;
  }
  else
  {
    texture_index = it - texture_views.begin();
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

void Batch::quad(const Quad* quad, const double x, const double y, const double z)
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

void Batch::text(Text& text, const double x, const double y, const double z)
{
  assert(m_current_vb != nullptr);

  if (!text.m_has_initialized)
  {
    text.m_font = AssetManager::get<Font>(text.typeface, m_context.device);
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

void Batch::nine_patch(NinePatch& nine_patch, const double x, const double y, const double z)
{
  assert(m_current_vb != nullptr);

  if (nine_patch.texture == nullptr)
  {
    nine_patch.texture = AssetManager::get<Texture>(nine_patch.resource_id, m_context.device);
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

void Batch::push_scissor(Vector4i scissor)
{
  // Try to reuse a vertex buffer
  if (vertex_buffers.size() > 1)
  {
    for (auto& vertex_buffer : vertex_buffers)
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
  vertex_buffers.push_back(std::move(vertex_buffer));
  m_current_vb = &vertex_buffers.back();
}

void Batch::pop_scissor() { m_current_vb = &vertex_buffers[0]; }
}  // namespace dl
