#include "./renderer2.hpp"

#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include <glm/gtc/matrix_transform.hpp>  // IWYU pragma: export

#include "./quad.hpp"
#include "./shader_program.hpp"
#include "./sprite.hpp"
#include "./texture.hpp"

namespace dl
{
Batch::Batch() {}

Batch::Batch(std::shared_ptr<ShaderProgram> shader, const int priority) : Layer(shader, priority) { load(); }

Batch::~Batch()
{
  glDeleteBuffers(1, &m_ebo);
  glDeleteBuffers(1, &m_vbo);
  glDeleteVertexArrays(1, &m_vao);
}

void Batch::load()
{
  m_vertices.reserve(INDICES_SIZE);

  glGenVertexArrays(1, &m_vao);
  glGenBuffers(1, &m_vbo);
  glGenBuffers(1, &m_ebo);

  glBindVertexArray(m_vao);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

  glBufferData(GL_ARRAY_BUFFER, BUFFER_SIZE, NULL, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(POSITION_INDEX, 3, GL_FLOAT, GL_FALSE, VERTEX_SIZE, (GLvoid*)0);
  glEnableVertexAttribArray(POSITION_INDEX);

  glVertexAttribPointer(TEXCOORDS_INDEX, 2, GL_FLOAT, GL_FALSE, VERTEX_SIZE, (GLvoid*)offsetof(VertexData, texcoords));
  glEnableVertexAttribArray(TEXCOORDS_INDEX);

  glVertexAttribPointer(
      TEXTURE_ID_INDEX, 1, GL_FLOAT, GL_FALSE, VERTEX_SIZE, (GLvoid*)offsetof(VertexData, texture_id));
  glEnableVertexAttribArray(TEXTURE_ID_INDEX);

  glVertexAttribPointer(COLOR_INDEX, 4, GL_UNSIGNED_BYTE, GL_TRUE, VERTEX_SIZE, (GLvoid*)offsetof(VertexData, color));
  glEnableVertexAttribArray(COLOR_INDEX);

  GLint offset = 0;
  GLuint indices[INDICES_SIZE];
  for (GLuint i = 0; i < INDICES_SIZE; i += 6)
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

void Batch::emplace(const std::shared_ptr<Renderable>& renderable, const double x, const double y, const double z)
{
  assert(m_index_count <= INDICES_SIZE);

  const auto sprite = std::dynamic_pointer_cast<Sprite>(renderable);

  if (sprite == nullptr)
  {
    spdlog::critical("Could not cast Renderable to Sprite");
    return;
  }

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
  auto general_transform = glm::mat4(1.0f);
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

void Batch::quad(const std::shared_ptr<Renderable>& renderable, const double x, const double y, const double z)
{
  assert(m_index_count <= INDICES_SIZE);

  const auto quad = std::dynamic_pointer_cast<Quad>(renderable);

  if (quad == nullptr)
  {
    spdlog::critical("Could not cast Renderable to Quad");
    return;
  }

  const auto color = quad->color.int_color;
  auto general_transform = glm::mat4(1.0f);
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

}  // namespace dl
