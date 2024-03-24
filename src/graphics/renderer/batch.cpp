#include "./batch.hpp"

#include <spdlog/spdlog.h>

#include "core/asset_manager.hpp"
#include "core/game_context.hpp"
#include "ecs/components/sprite.hpp"
#include "ecs/components/texture_slice.hpp"
#include "ecs/components/tile.hpp"
#include "graphics/camera.hpp"
#include "graphics/color.hpp"
#include "graphics/display.hpp"
#include "graphics/nine_patch.hpp"
#include "graphics/quad.hpp"
#include "graphics/render_face.hpp"
#include "graphics/renderer/spritesheet.hpp"
#include "graphics/renderer/utils.hpp"
#include "graphics/text.hpp"

namespace dl
{
Batch::Batch(GameContext& game_context)
    : m_game_context(game_context),
      m_context(m_game_context.display->wgpu_context),
      m_dummy_texture(Texture::dummy(m_context.device))
{
}

void Batch::load()
{
  m_load_batch_data();
  m_load_textures();
}

void Batch::m_load_batch_data()
{
  // Add main vertex buffer
  batch_data.emplace_back(m_context.device, MAIN_BATCH_VERTEX_COUNT, MAIN_BATCH_INDEX_COUNT);
  m_current_vb = &batch_data[0];
  utils::populate_quad_index_buffer(m_context.queue, m_current_vb->index_buffer, MAIN_BATCH_INDEX_COUNT);
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

bool Batch::empty()
{
  for (const auto& batch_datum : batch_data)
  {
    if (batch_datum.index_buffer_count > 0)
    {
      return false;
    }
  }

  return true;
}

void Batch::clear_textures() { m_texture_slot_index = m_texture_slot_index_base; }

uint32_t Batch::pin_texture(WGPUTextureView texture_view)
{
  const uint32_t slot_index = m_texture_slot_index_base;
  texture_views[m_texture_slot_index_base] = texture_view;
  ++m_texture_slot_index_base;
  clear_textures();

  return slot_index;
}

void Batch::sprite(Sprite& sprite, const double x, const double y, const double z, const RenderFace face)
{
  assert(sprite.spritesheet != nullptr);
  assert(sprite.frame_data != nullptr);
  assert(sprite.size.x != 0 && sprite.size.y != 0);

  const glm::vec2& size = sprite.size;
  const auto& texture_coordinates = sprite.spritesheet->get_uv_coordinates(sprite.frame_data->faces[face]);
  unsigned int color = sprite.color.int_color;

  if (sprite.color.opacity_factor < 1.0)
  {
    const auto& sprite_color = sprite.color.rgba_color;
    color = Color::rgba_to_int(sprite_color.r,
                               sprite_color.g,
                               sprite_color.b,
                               static_cast<uint8_t>(sprite_color.a * sprite.color.opacity_factor));
  }

  const float texture_index = m_get_texture_index(sprite.spritesheet->texture->view);

  SpriteBatchData data{face, glm::vec3{x, y, z}, size, texture_coordinates, color, texture_index};

  m_emplace_sprite_face(std::move(data));
}

void Batch::texture_slice(TextureSlice& slice, const double x, const double y, const double z)
{
  assert(slice.texture != nullptr);
  assert(slice.size.x != 0 && slice.size.y != 0);

  const glm::vec2& size = slice.size;
  const auto& uv_coordinates = slice.uv_coordinates;
  unsigned int color = slice.color.int_color;

  if (slice.color.opacity_factor < 1.0)
  {
    const auto& slice_color = slice.color.rgba_color;
    color = Color::rgba_to_int(
        slice_color.r, slice_color.g, slice_color.b, static_cast<uint8_t>(slice_color.a * slice.color.opacity_factor));
  }

  const float texture_index = m_get_texture_index(slice.texture->view);

  // Top left vertex
  m_current_vb->emplace(glm::vec3{x, y, z}, uv_coordinates[0], texture_index, color);

  // Top right vertex
  m_current_vb->emplace(glm::vec3{x + size.x, y, z}, uv_coordinates[1], texture_index, color);

  // Bottom left vertex
  m_current_vb->emplace(glm::vec3{x, y + size.y, z}, uv_coordinates[3], texture_index, color);

  // Bottom right vertex
  m_current_vb->emplace(glm::vec3{x + size.x, y + size.y, z}, uv_coordinates[2], texture_index, color);

  m_current_vb->index_buffer_count += 6;
}

void Batch::tile(const Tile& tile, const double x, const double y, const double z, const RenderFace face)
{
  assert(m_current_vb != nullptr);
  assert(tile.spritesheet != nullptr);
  assert(tile.frame_data != nullptr);
  assert(tile.size.x != 0);
  assert(tile.size.y != 0);

  const auto& size = tile.size;
  const uint32_t color = 0xFFFFFFFF;
  const auto& uv_coordinates = tile.spritesheet->get_uv_coordinates(tile.frame_data->faces[face]);

  const float texture_index = m_get_texture_index(tile.spritesheet->texture->view);

  SpriteBatchData data{face, glm::vec3{x, y, z}, size, uv_coordinates, color, texture_index};

  m_emplace_sprite_face(std::move(data));
}

void Batch::texture(const Texture& texture, const double x, const double y, const double z)
{
  assert(m_current_vb != nullptr);
  assert(texture.size.x != 0);
  assert(texture.size.y != 0);

  const auto& size = texture.size;
  const uint32_t color = 0xFFFFFFFF;

  const float texture_index = m_get_texture_index(texture.view);

  // Top left vertex
  m_current_vb->emplace(glm::vec3{x, y, z}, glm::vec2{0.0f, 0.0f}, texture_index, color);

  // Top right vertex
  m_current_vb->emplace(glm::vec3{x + size.x, y, z}, glm::vec2{1.0f, 0.0f}, texture_index, color);

  // Bottom left vertex
  m_current_vb->emplace(glm::vec3{x, y + size.y, z}, glm::vec2{0.0f, 1.0f}, texture_index, color);

  // Bottom right vertex
  m_current_vb->emplace(glm::vec3{x + size.x, y + size.y, z}, glm::vec2{1.0f, 1.0f}, texture_index, color);

  m_current_vb->index_buffer_count += 6;
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

  // Bottom right vertex
  m_current_vb->emplace(glm::vec3{x + quad->w, y + quad->h, z}, glm::vec2{0}, -1.0f, color);

  m_current_vb->index_buffer_count += 6;
}

void Batch::text(Text& text, const double x, const double y, const double z)
{
  assert(m_current_vb != nullptr);

  if (!text.has_initialized)
  {
    assert(m_game_context.asset_manager != nullptr);
    text.initialize(*m_game_context.asset_manager);
  }
  else if (!text.get_is_static())
  {
    text.update();
  }

  for (auto& character : text.characters)
  {
    // Character is a space
    if (character.slice == nullptr)
    {
      continue;
    }

    if (character.slice->color.opacity_factor != text.color.opacity_factor)
    {
      character.slice->color.opacity_factor = text.color.opacity_factor;
    }

    texture_slice(*character.slice, character.x + x, character.y + y, z);
  }
}

void Batch::nine_patch(NinePatch& nine_patch, const double x, const double y, const double z)
{
  assert(m_current_vb != nullptr);

  if (nine_patch.texture_atlas == nullptr)
  {
    nine_patch.texture_atlas = m_game_context.asset_manager->get<TextureAtlas>(nine_patch.resource_id);
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
  texture_slice(nine_patch.border_patches[0], x, y, z);
  // Top center patch
  texture_slice(nine_patch.border_patches[1], x + nine_patch.border, y, z);
  // Top right patch
  texture_slice(nine_patch.border_patches[2], x + nine_patch.size.x - nine_patch.border, y, z);
  // Center right patch
  texture_slice(nine_patch.border_patches[3], x + nine_patch.size.x - nine_patch.border, y + nine_patch.border, z);
  // Bottom right patch
  texture_slice(nine_patch.border_patches[4],
                x + nine_patch.size.x - nine_patch.border,
                y + nine_patch.size.y - nine_patch.border,
                z);
  // Bottom center patch
  texture_slice(nine_patch.border_patches[5], x + nine_patch.border, y + nine_patch.size.y - nine_patch.border, z);
  // Bottom left patch
  texture_slice(nine_patch.border_patches[6], x, y + nine_patch.size.y - nine_patch.border, z);
  // Center left patch
  texture_slice(nine_patch.border_patches[7], x, y + nine_patch.border, z);

  // Center patch
  texture_slice(nine_patch.center_patch, x + nine_patch.border, y + nine_patch.border, z);
}

void Batch::push_scissor(Vector4i scissor)
{
  // Scale scissor if using High DPI mode
  const auto& scale = Display::get_pixel_scale();

  if (scale.x > 1.0 || scale.y > 1.0)
  {
    scissor.x *= scale.x;
    scissor.z *= scale.x;
    scissor.y *= scale.x;
    scissor.w *= scale.x;
  }

  // Try to reuse a vertex buffer
  if (batch_data.size() > 1)
  {
    for (auto& batch_datum : batch_data)
    {
      if (batch_datum.index_buffer_count == 0)
      {
        batch_datum.scissor = std::move(scissor);
        m_current_vb = &batch_datum;
        return;
      }
    }
  }

  // Create a new buffer
  BatchData<VertexData> batch_datum{m_context.device, SECONDARY_BATCH_VERTEX_COUNT, SECONDARY_BATCH_INDEX_COUNT};
  batch_datum.scissor = std::move(scissor);
  batch_data.push_back(std::move(batch_datum));
  m_current_vb = &batch_data.back();
  utils::populate_quad_index_buffer(m_context.queue, m_current_vb->index_buffer, SECONDARY_BATCH_INDEX_COUNT);
}

void Batch::pop_scissor() { m_current_vb = &batch_data[0]; }

void Batch::m_emplace_sprite_face(const SpriteBatchData data)
{
  switch (data.face)
  {
  case DL_RENDER_FACE_TOP:
    m_emplace_sprite_face_top(std::move(data));
    break;
  case DL_RENDER_FACE_FRONT:
    m_emplace_sprite_face_front(std::move(data));
    break;
  case DL_RENDER_FACE_BOTTOM:
    // TODO
    break;
  case DL_RENDER_FACE_LEFT:
    // TODO
    break;
  case DL_RENDER_FACE_RIGHT:
    // TODO
    break;
  case DL_RENDER_FACE_BACK:
    // TODO
    break;
  case DL_RENDER_FACE_CENTER_HORIZONTAL:
    // TODO
    break;
  case DL_RENDER_FACE_CENTER_VERTICAL:
    // TODO
    break;
  default:
    break;
  }
}

void Batch::m_emplace_sprite_face_top(const SpriteBatchData data)
{
  // Top left vertex
  m_current_vb->emplace(data.position, data.texture_coordinates[0], data.texture_index, data.color);

  // Top right vertex
  m_current_vb->emplace(glm::vec3{data.position.x + data.size.x, data.position.y, data.position.z},
                        data.texture_coordinates[1],
                        data.texture_index,
                        data.color);

  // Bottom left vertex
  m_current_vb->emplace(glm::vec3{data.position.x, data.position.y + data.size.y, data.position.z},
                        data.texture_coordinates[3],
                        data.texture_index,
                        data.color);

  // Bottom right vertex
  m_current_vb->emplace(glm::vec3{data.position.x + data.size.x, data.position.y + data.size.y, data.position.z},
                        data.texture_coordinates[2],
                        data.texture_index,
                        data.color);

  m_current_vb->index_buffer_count += 6;
}

void Batch::m_emplace_sprite_face_front(const SpriteBatchData data)
{
  // Top left vertex
  m_current_vb->emplace(glm::vec3{data.position.x, data.position.y + data.size.y, data.position.z + data.size.y},
                        data.texture_coordinates[0],
                        data.texture_index,
                        data.color);

  // Top right vertex
  m_current_vb->emplace(
      glm::vec3{data.position.x + data.size.x, data.position.y + data.size.y, data.position.z + data.size.y},
      data.texture_coordinates[1],
      data.texture_index,
      data.color);

  // Bottom left vertex
  m_current_vb->emplace(glm::vec3{data.position.x, data.position.y + data.size.y, data.position.z},
                        data.texture_coordinates[3],
                        data.texture_index,
                        data.color);

  // Bottom right vertex
  m_current_vb->emplace(glm::vec3{data.position.x + data.size.x, data.position.y + data.size.y, data.position.z},
                        data.texture_coordinates[2],
                        data.texture_index,
                        data.color);

  m_current_vb->index_buffer_count += 6;
}

// Build vector of textures to bind when rendering
// texture_index is the index in texture_views that will
// be translated to a index in the shader.
float Batch::m_get_texture_index(const WGPUTextureView texture_view)
{
  float texture_index = 0.00f;
  const auto upper_bound = texture_views.begin() + m_texture_slot_index;
  const auto it = std::find(texture_views.begin(), upper_bound, texture_view);

  if (it >= upper_bound)
  {
    texture_index = static_cast<float>(m_texture_slot_index);
    texture_views[m_texture_slot_index] = texture_view;
    ++m_texture_slot_index;
    should_update_texture_bind_group = true;
  }
  else
  {
    texture_index = it - texture_views.begin();
  }

  return texture_index;
}

}  // namespace dl
