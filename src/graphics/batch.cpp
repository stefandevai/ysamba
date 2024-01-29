#include "./batch.hpp"

#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include <glm/gtc/matrix_transform.hpp>  // IWYU pragma: export
#include <glm/gtc/type_ptr.hpp>

#include "./multi_sprite.hpp"
#include "./nine_patch.hpp"
#include "./quad.hpp"
#include "./shader_program.hpp"
#include "./sprite.hpp"
#include "./text.hpp"
#include "./texture.hpp"
#include "./tile_render_data.hpp"

namespace dl
{
Batch::Batch(const int priority) : priority(priority) {}

Batch::Batch(const std::string& shader_id, const int priority) : shader_id(shader_id), priority(priority) { load(); }

Batch::~Batch()
{
  glDeleteBuffers(1, &m_ebo);
  glDeleteBuffers(1, &m_vbo);
  glDeleteVertexArrays(1, &m_vao);
}

void Batch::load()
{
  m_vertices.reserve(m_indices_size);

  // Push a default view matrix for an orthographic projection
  // as a fallback
  std::array<float, 16> values{
      1.0f,
      0.0f,
      0.0f,
      0.0f,
      0.0f,
      1.0f,
      0.0f,
      0.0f,
      0.0f,
      0.0f,
      1.0f,
      0.0f,
      0.0f,
      0.0f,
      -1.0f,
      1.0f,
  };
  glm::mat4 default_view_matrix = glm::make_mat4(values.data());
  push_matrix(default_view_matrix);

  glGenVertexArrays(1, &m_vao);
  glGenBuffers(1, &m_vbo);
  glGenBuffers(1, &m_ebo);

  glBindVertexArray(m_vao);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

  glBufferData(GL_ARRAY_BUFFER, m_buffer_size, nullptr, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(PositionIndex, 3, GL_FLOAT, GL_FALSE, m_vertex_size, (GLvoid*)0);
  glEnableVertexAttribArray(PositionIndex);

  glVertexAttribPointer(
      TextureCoordsIndex, 2, GL_FLOAT, GL_FALSE, m_vertex_size, (GLvoid*)offsetof(VertexData, texture_coordinates));
  glEnableVertexAttribArray(TextureCoordsIndex);

  glVertexAttribPointer(
      TextureIdIndex, 1, GL_FLOAT, GL_FALSE, m_vertex_size, (GLvoid*)offsetof(VertexData, texture_id));
  glEnableVertexAttribArray(TextureIdIndex);

  glVertexAttribPointer(ColorIndex, 4, GL_UNSIGNED_BYTE, GL_TRUE, m_vertex_size, (GLvoid*)offsetof(VertexData, color));
  glEnableVertexAttribArray(ColorIndex);

  GLint offset = 0;
  GLuint indices[m_indices_size];
  for (GLuint i = 0; i < m_indices_size; i += 6)
  {
    indices[i] = offset;
    indices[i + 1] = offset + 1;
    indices[i + 2] = offset + 2;

    indices[i + 3] = offset;
    indices[i + 4] = offset + 2;
    indices[i + 5] = offset + 3;

    offset += 4;
  }
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  glBindVertexArray(0);
}

void Batch::render()
{
  // Return early if no sprites were added to the batch
  if (index_count == 0)
  {
    return;
  }

  // Copy vertex data to the GPU
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferSubData(GL_ARRAY_BUFFER, 0, (m_vertices_index + 1) * sizeof(VertexData), m_vertices.data());

  for (unsigned int i = 0; i < m_textures.size(); ++i)
  {
    glActiveTexture(GL_TEXTURE0 + i);
    m_textures[i]->bind();
    shader->set_int("textures[" + std::to_string(i) + "]", i);
  }
  glActiveTexture(GL_TEXTURE0);

  glBindVertexArray(m_vao);
  glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);
  index_count = 0;
  m_vertices_index = 0;
}

void Batch::push_matrix(const glm::mat4& matrix)
{
  m_matrix_stack.push_back(m_matrix);
  m_matrix = matrix * m_matrix;
}

const glm::mat4 Batch::pop_matrix()
{
  auto old_matrix = m_matrix;
  m_matrix = m_matrix_stack.back();
  m_matrix_stack.pop_back();
  return old_matrix;
}

const glm::mat4& Batch::peek_matrix() { return m_matrix; }

void Batch::tile(const TileRenderData& tile, const double x, const double y, const double z)
{
  /* assert(index_count <= m_indices_size); */

  const auto& size = tile.size;
  const auto& uv_coordinates = tile.uv_coordinates;
  const uint32_t color = 0xFFFFFFFF;

  /* assert(tile.texture != nullptr); */
  /* assert(size.x != 0); */
  /* assert(size.y != 0); */

  float texture_index = 0.00f;
  const auto it = std::find(m_textures.begin(), m_textures.end(), tile.texture);
  if (it == m_textures.end())
  {
    texture_index = m_textures.size();
    m_textures.push_back(tile.texture);
  }
  else
  {
    texture_index = it - m_textures.begin();
  }

  // Top left vertex
  if (tile.frame_data->angle == FrameAngle::Parallel)
  {
    m_vertices[m_vertices_index++] = VertexData{glm::vec3{x, y, z}, uv_coordinates[0], texture_index, color};
  }
  else
  {
    m_vertices[m_vertices_index++] =
        VertexData{glm::vec3{x, y + size.y, z + size.y}, uv_coordinates[0], texture_index, color};
  }

  // Top right vertex
  if (tile.frame_data->angle == FrameAngle::Parallel)
  {
    m_vertices[m_vertices_index++] = VertexData{glm::vec3{x + size.x, y, z}, uv_coordinates[1], texture_index, color};
  }
  else
  {
    m_vertices[m_vertices_index++] =
        VertexData{glm::vec3{x + size.x, y + size.y, z + size.y}, uv_coordinates[1], texture_index, color};
  }

  // Bottom right vertex
  m_vertices[m_vertices_index++] =
      VertexData{glm::vec3{x + size.x, y + size.y, z}, uv_coordinates[2], texture_index, color};

  // Bottom left vertex
  m_vertices[m_vertices_index++] = VertexData{glm::vec3{x, y + size.y, z}, uv_coordinates[3], texture_index, color};

  // Each quad has 6 vertices, we have therefore to increment by 6 each time
  index_count += 6;
}

void Batch::emplace(Sprite* sprite, const double x, const double y, const double z)
{
  assert(index_count <= m_indices_size);

  const glm::vec2& size = sprite->get_size();
  const auto& texture_coordinates = sprite->get_texture_coordinates();
  unsigned int color = sprite->color.int_color;

  const std::shared_ptr<Texture>& texture = sprite->texture;

  assert(sprite->texture != nullptr);
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
  // texture_index is the index in m_textures that will
  // be translated to a index in the shader.
  float texture_index = 0.00f;
  auto texture_ptr = texture.get();
  const auto it = std::find(m_textures.begin(), m_textures.end(), texture_ptr);
  if (it == m_textures.end())
  {
    texture_index = m_textures.size();
    m_textures.push_back(texture_ptr);
  }
  else
  {
    texture_index = it - m_textures.begin();
  }

  // Top left vertex
  if (sprite->frame_angle == FrameAngle::Parallel)
  {
    m_vertices[m_vertices_index++] = VertexData{glm::vec3{x, y, z}, texture_coordinates[0], texture_index, color};
  }
  else
  {
    m_vertices[m_vertices_index++] =
        VertexData{glm::vec3{x, y + size.y, z + size.y}, texture_coordinates[0], texture_index, color};
  }

  // Top right vertex
  if (sprite->frame_angle == FrameAngle::Parallel)
  {
    m_vertices[m_vertices_index++] =
        VertexData{glm::vec3{x + size.x, y, z}, texture_coordinates[1], texture_index, color};
  }
  else
  {
    m_vertices[m_vertices_index++] =
        VertexData{glm::vec3{x + size.x, y + size.y, z + size.y}, texture_coordinates[1], texture_index, color};
  }

  // Bottom right vertex
  m_vertices[m_vertices_index++] =
      VertexData{glm::vec3{x + size.x, y + size.y, z}, texture_coordinates[2], texture_index, color};

  // Bottom left vertex
  m_vertices[m_vertices_index++] =
      VertexData{glm::vec3{x, y + size.y, z}, texture_coordinates[3], texture_index, color};

  // Each quad has 6 vertices, we have therefore to increment by 6 each time
  index_count += 6;
}

void Batch::emplace(const MultiSprite* sprite, const double x, const double y, const double z)
{
  assert(index_count <= m_indices_size);

  unsigned int color = sprite->color.int_color;

  const std::shared_ptr<Texture>& texture = sprite->texture;

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
  // texture_index is the index in m_textures that will
  // be translated to a index in the shader.
  float texture_index = 0.00f;
  auto texture_ptr = texture.get();
  const auto it = std::find(m_textures.begin(), m_textures.end(), texture_ptr);
  if (it == m_textures.end())
  {
    texture_index = m_textures.size();
    m_textures.push_back(texture_ptr);
  }
  else
  {
    texture_index = it - m_textures.begin();
  }

  const auto& size = sprite->get_size();
  const std::array<glm::vec2, 4> texture_coordinates = sprite->get_texture_coordinates();
  const int frame_width = texture->get_frame_width() * (size.x);
  const int frame_height = texture->get_frame_height() * (size.y);

  // Top left vertex
  // Change according to the angle that the quad will be rendered
  if (sprite->frame_angle == FrameAngle::Parallel)
  {
    m_vertices[m_vertices_index++] = VertexData{glm::vec3{x, y, z}, texture_coordinates[0], texture_index, color};
  }
  else
  {
    m_vertices[m_vertices_index++] =
        VertexData{glm::vec3{x, y + frame_height, z + frame_height}, texture_coordinates[0], texture_index, color};
  }

  // Top right vertex
  if (sprite->frame_angle == FrameAngle::Parallel)
  {
    m_vertices[m_vertices_index++] =
        VertexData{glm::vec3{x + frame_width, y, z}, texture_coordinates[1], texture_index, color};
  }
  else
  {
    m_vertices[m_vertices_index++] = VertexData{
        glm::vec3{x + frame_width, y + frame_height, z + frame_height}, texture_coordinates[1], texture_index, color};
  }

  // Bottom right vertex
  m_vertices[m_vertices_index++] =
      VertexData{glm::vec3{x + frame_width, y + frame_height, z}, texture_coordinates[2], texture_index, color};

  // Bottom left vertex
  m_vertices[m_vertices_index++] =
      VertexData{glm::vec3{x, y + frame_height, z}, texture_coordinates[3], texture_index, color};

  // Each quad has 6 vertices, we have therefore to increment by 6 each time
  index_count += 6;
}

void Batch::quad(const Quad* quad, const double x, const double y, const double z)
{
  assert(index_count <= m_indices_size);

  uint32_t color = quad->color.int_color;

  if (quad->color.opacity_factor < 1.0)
  {
    const auto& quad_color = quad->color.rgba_color;
    color = Color::rgba_to_int(
        quad_color.r, quad_color.g, quad_color.b, static_cast<uint8_t>(quad_color.a * quad->color.opacity_factor));
  }

  // Top left vertex
  m_vertices[m_vertices_index++] = VertexData{glm::vec3{x, y, z}, glm::vec2{0}, -1, color};

  // Top right vertex
  m_vertices[m_vertices_index++] = VertexData{glm::vec3{x + quad->w, y, z}, glm::vec2{0}, -1, color};

  // Bottom right vertex
  m_vertices[m_vertices_index++] = VertexData{glm::vec3{x + quad->w, y + quad->h, z}, glm::vec2{0}, -1, color};

  // Bottom left vertex
  m_vertices[m_vertices_index++] = VertexData{glm::vec3{x, y + quad->h, z}, glm::vec2{0}, -1, color};

  // Each quad has 6 vertices, we have therefore to increment by 6 each time
  index_count += 6;
}

void Batch::text(Text& text, const double x, const double y, const double z)
{
  if (!text.get_has_initialized())
  {
    text.initialize();
  }

  if (!text.get_is_static())
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

    emplace(character.sprite.get(), character.x + x, character.y + y, z);
  }
}

void Batch::nine_patch(NinePatch& nine_patch, const double x, const double y, const double z)
{
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
  emplace(&nine_patch.border_patches[0], x, y, z);
  // Top center patch
  emplace(&nine_patch.border_patches[1], x + nine_patch.border, y, z);
  // Top right patch
  emplace(&nine_patch.border_patches[2], x + nine_patch.size.x - nine_patch.border, y, z);
  // Center right patch
  emplace(&nine_patch.border_patches[3], x + nine_patch.size.x - nine_patch.border, y + nine_patch.border, z);
  // Bottom right patch
  emplace(&nine_patch.border_patches[4],
          x + nine_patch.size.x - nine_patch.border,
          y + nine_patch.size.y - nine_patch.border,
          z);
  // Bottom center patch
  emplace(&nine_patch.border_patches[5], x + nine_patch.border, y + nine_patch.size.y - nine_patch.border, z);
  // Bottom left patch
  emplace(&nine_patch.border_patches[6], x, y + nine_patch.size.y - nine_patch.border, z);
  // Center left patch
  emplace(&nine_patch.border_patches[7], x, y + nine_patch.border, z);

  // Center patch
  emplace(&nine_patch.center_patch, x + nine_patch.border, y + nine_patch.border, z);
}

void Batch::add_scissor(const Vector4i& scissor)
{
  this->scissor = scissor;
  has_scissor = true;
}

void Batch::clear_textures() { m_textures.clear(); }

}  // namespace dl
