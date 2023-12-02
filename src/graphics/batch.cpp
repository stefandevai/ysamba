#include "./batch.hpp"

#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include <glm/gtc/matrix_transform.hpp>  // IWYU pragma: export

#include "./multi_sprite.hpp"
#include "./quad.hpp"
#include "./shader_program.hpp"
#include "./sprite.hpp"
#include "./text.hpp"
#include "./texture.hpp"

namespace dl
{
Batch::Batch(const int priority) : priority(priority) {}

Batch::Batch(std::shared_ptr<ShaderProgram> shader, const int priority) : shader(shader), priority(priority) { load(); }

Batch::~Batch()
{
  glDeleteBuffers(1, &m_ebo);
  glDeleteBuffers(1, &m_vbo);
  glDeleteVertexArrays(1, &m_vao);
}

void Batch::load()
{
  m_vertices.reserve(m_indices_size);

  glGenVertexArrays(1, &m_vao);
  glGenBuffers(1, &m_vbo);
  glGenBuffers(1, &m_ebo);

  glBindVertexArray(m_vao);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

  glBufferData(GL_ARRAY_BUFFER, m_buffer_size, NULL, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(PositionIndex, 3, GL_FLOAT, GL_FALSE, m_vertex_size, (GLvoid*)0);
  glEnableVertexAttribArray(PositionIndex);

  glVertexAttribPointer(
      TextureCoordsIndex, 2, GL_FLOAT, GL_FALSE, m_vertex_size, (GLvoid*)offsetof(VertexData, texcoords));
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
  if (m_index_count == 0)
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
  glDrawElements(GL_TRIANGLES, m_index_count, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
  m_index_count = 0;
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

void Batch::emplace(const std::shared_ptr<Sprite>& sprite, const double x, const double y, const double z)
{
  assert(m_index_count <= m_indices_size);

  const glm::vec2 size = sprite->get_size();
  const std::array<glm::vec2, 4> texcoords = sprite->get_texcoords();
  unsigned int color = 4294967295;  // Default white color

  const std::shared_ptr<Texture>& texture = sprite->texture;

  assert(sprite->texture != nullptr);
  assert(size.x != 0);
  assert(size.y != 0);

  if (sprite->color)
  {
    color = sprite->color->int_color;
  }

  // Build vector of textures to bind when rendering
  // texture_index is the index in m_textures that will
  // be translated to a index in the shader.
  float texture_index = 0.f;
  const auto it = std::find(m_textures.begin(), m_textures.end(), texture);
  if (it == m_textures.end())
  {
    texture_index = m_textures.size();
    m_textures.emplace_back(texture);
  }
  else
  {
    texture_index = it - m_textures.begin();
  }

  // Get transformations and apply them to the sprite vertices
  auto general_transform = m_matrix;
  if (sprite->transform)
  {
    // Translate and add pivot for other transformations
    general_transform = glm::translate(
        general_transform,
        glm::vec3(x + sprite->transform->pivot.x, y + sprite->transform->pivot.y, z + sprite->transform->pivot.z));

    // Scale only if necessary
    if (sprite->transform->scale.x != 1.f || sprite->transform->scale.y != 1.f || sprite->transform->scale.z != 1.f)
    {
      general_transform = glm::scale(general_transform, sprite->transform->scale);
    }

    // Rotate only if necessary
    if (sprite->transform->rotation.x != 0.0f)
    {
      general_transform =
          glm::rotate(general_transform, glm::radians(sprite->transform->rotation.x), glm::vec3(1.f, 0.f, 0.f));
    }
    if (sprite->transform->rotation.y != 0.0f)
    {
      general_transform =
          glm::rotate(general_transform, glm::radians(sprite->transform->rotation.y), glm::vec3(0.f, 1.f, 0.f));
    }
    if (sprite->transform->rotation.z != 0.0f)
    {
      general_transform =
          glm::rotate(general_transform, glm::radians(sprite->transform->rotation.z), glm::vec3(0.f, 0.f, 1.f));
    }

    // Remove the pivot translation if needed
    if (sprite->transform->pivot.x != 0.f || sprite->transform->pivot.y != 0.f || sprite->transform->pivot.z != 0.f)
    {
      general_transform = glm::translate(general_transform, sprite->transform->pivot * -1.f);
    }
  }
  else
  {
    general_transform = glm::translate(general_transform, glm::vec3(x, y, z));
  }

  // Top left vertex
  glm::vec4 transformation_result = general_transform * glm::vec4(0.f, 0.f, 1.f, 1.f);
  m_vertices[m_vertices_index++] =
      VertexData{glm::vec3{transformation_result.x, transformation_result.y, transformation_result.z},
                 texcoords[0],
                 texture_index,
                 color};

  // Top right vertex
  transformation_result = general_transform * glm::vec4(size.x, 0.f, 1.f, 1.f);
  m_vertices[m_vertices_index++] =
      VertexData{glm::vec3{transformation_result.x, transformation_result.y, transformation_result.z},
                 texcoords[1],
                 texture_index,
                 color};

  // Bottom right vertex
  transformation_result = general_transform * glm::vec4(size.x, size.y, 1.f, 1.f);
  m_vertices[m_vertices_index++] =
      VertexData{glm::vec3{transformation_result.x, transformation_result.y, transformation_result.z},
                 texcoords[2],
                 texture_index,
                 color};

  // Bottom left vertex
  transformation_result = general_transform * glm::vec4(0.f, size.y, 1.f, 1.f);
  m_vertices[m_vertices_index++] =
      VertexData{glm::vec3{transformation_result.x, transformation_result.y, transformation_result.z},
                 texcoords[3],
                 texture_index,
                 color};

  // Each quad has 6 vertices, we have therefore to increment by 6 each time
  m_index_count += 6;
}

void Batch::emplace(const std::shared_ptr<MultiSprite>& sprite, const double x, const double y, const double z)
{
  assert(m_index_count <= m_indices_size);

  unsigned int color = 4294967295;  // Default white color

  const std::shared_ptr<Texture>& texture = sprite->texture;

  assert(sprite->texture != nullptr);

  if (sprite->color)
  {
    color = sprite->color->int_color;
  }

  // Build vector of textures to bind when rendering
  // texture_index is the index in m_textures that will
  // be translated to a index in the shader.
  float texture_index = 0.f;
  const auto it = std::find(m_textures.begin(), m_textures.end(), texture);
  if (it == m_textures.end())
  {
    texture_index = m_textures.size();
    m_textures.emplace_back(texture);
  }
  else
  {
    texture_index = it - m_textures.begin();
  }

  const auto& frames = sprite->get_frames();
  const auto& size = sprite->get_size();
  const int frame_width = texture->get_frame_width();
  const int frame_height = texture->get_frame_height();

  for (size_t i = 0; i < size.x; ++i)
  {
    for (size_t j = 0; j < size.y; ++j)
    {
      const std::array<glm::vec2, 4> texcoords = sprite->get_texcoords(frames[j * size.x + i]);

      // Get transformations and apply them to the sprite vertices
      auto general_transform = m_matrix;
      general_transform = glm::translate(general_transform, glm::vec3(x + i * frame_width, y + j * frame_height, z));

      // Top left vertex
      glm::vec4 transformation_result = general_transform * glm::vec4(0.f, 0.f, 1.f, 1.f);
      m_vertices[m_vertices_index++] =
          VertexData{glm::vec3{transformation_result.x, transformation_result.y, transformation_result.z},
                     texcoords[0],
                     texture_index,
                     color};

      // Top right vertex
      transformation_result = general_transform * glm::vec4(frame_width, 0.f, 1.f, 1.f);
      m_vertices[m_vertices_index++] =
          VertexData{glm::vec3{transformation_result.x, transformation_result.y, transformation_result.z},
                     texcoords[1],
                     texture_index,
                     color};

      // Bottom right vertex
      transformation_result = general_transform * glm::vec4(frame_width, frame_height, 1.f, 1.f);
      m_vertices[m_vertices_index++] =
          VertexData{glm::vec3{transformation_result.x, transformation_result.y, transformation_result.z},
                     texcoords[2],
                     texture_index,
                     color};

      // Bottom left vertex
      transformation_result = general_transform * glm::vec4(0.f, frame_height, 1.f, 1.f);
      m_vertices[m_vertices_index++] =
          VertexData{glm::vec3{transformation_result.x, transformation_result.y, transformation_result.z},
                     texcoords[3],
                     texture_index,
                     color};

      // Each quad has 6 vertices, we have therefore to increment by 6 each time
      m_index_count += 6;
    }
  }
}

void Batch::quad(const std::shared_ptr<Quad>& quad, const double x, const double y, const double z)
{
  assert(m_index_count <= m_indices_size);

  const auto color = quad->color.int_color;
  auto general_transform = m_matrix;
  general_transform = glm::translate(general_transform, glm::vec3(x, y, z));

  // Top left vertex
  glm::vec4 transformation_result = general_transform * glm::vec4(0.f, 0.f, 1.f, 1.f);
  m_vertices[m_vertices_index++] = VertexData{
      glm::vec3{transformation_result.x, transformation_result.y, transformation_result.z}, glm::vec2{0}, -1, color};

  // Top right vertex
  transformation_result = general_transform * glm::vec4(quad->w, 0.f, 1.f, 1.f);
  m_vertices[m_vertices_index++] = VertexData{
      glm::vec3{transformation_result.x, transformation_result.y, transformation_result.z}, glm::vec2{0}, -1, color};

  // Bottom right vertex
  transformation_result = general_transform * glm::vec4(quad->w, quad->h, 1.f, 1.f);
  m_vertices[m_vertices_index++] = VertexData{
      glm::vec3{transformation_result.x, transformation_result.y, transformation_result.z}, glm::vec2{0}, -1, color};

  // Bottom left vertex
  transformation_result = general_transform * glm::vec4(0.f, quad->h, 1.f, 1.f);
  m_vertices[m_vertices_index++] = VertexData{
      glm::vec3{transformation_result.x, transformation_result.y, transformation_result.z}, glm::vec2{0}, -1, color};

  // Each quad has 6 vertices, we have therefore to increment by 6 each time
  m_index_count += 6;
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

    emplace(character.sprite, character.x + x, character.y + y, z);
  }
}

void Batch::add_scissor(const Vector4i& scissor)
{
  this->scissor = scissor;
  has_scissor = true;
}

}  // namespace dl
