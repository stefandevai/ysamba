#include "./batch_quad.hpp"

#include <iostream>
#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp> // IWYU pragma: export
#include "./texture.hpp"
#include "./shader_program.hpp"
#include "./sprite.hpp"

namespace dl
{
BatchQuad::BatchQuad(std::shared_ptr<ShaderProgram> shader)
  : Layer(shader)
{
  glGenVertexArrays (1, &m_vao);
  glGenBuffers (1, &m_vbo);
  glGenBuffers (1, &m_ebo);

  glBindVertexArray (m_vao);
  glBindBuffer (GL_ARRAY_BUFFER, m_vbo);

  glBufferData (GL_ARRAY_BUFFER, BUFFER_SIZE, NULL, GL_DYNAMIC_DRAW);
  glVertexAttribPointer (POSITION_INDEX, 3, GL_FLOAT, GL_FALSE, VERTEX_SIZE, (GLvoid*) 0);
  glEnableVertexAttribArray (POSITION_INDEX);

  glVertexAttribPointer (COLOR_INDEX, 4, GL_UNSIGNED_BYTE, GL_TRUE, VERTEX_SIZE, (GLvoid*) offsetof (VertexData, color));
  glEnableVertexAttribArray (COLOR_INDEX);

  GLint offset = 0;
  GLuint indices[INDICES_SIZE];
  for (GLuint i = 0; i < INDICES_SIZE; i += 6)
  {
    indices[i]     = offset;
    indices[i + 1] = offset + 1;
    indices[i + 2] = offset + 2;

    indices[i + 3] = offset;
    indices[i + 4] = offset + 2;
    indices[i + 5] = offset + 3;

    offset += 4;
  }
  glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, m_ebo);
  glBufferData (GL_ELEMENT_ARRAY_BUFFER, sizeof (indices), indices, GL_STATIC_DRAW);

  glBindVertexArray (0);
}

BatchQuad::~BatchQuad()
{
  glDeleteBuffers (1, &m_ebo);
  glDeleteBuffers (1, &m_vbo);
  glDeleteVertexArrays (1, &m_vao);
}

void BatchQuad::render ()
{
  // Return early if no sprites were added to the batch
  if (m_index_count == 0)
  {
    return;
  }

  /* for (unsigned int i = 0; i < m_textures.size(); ++i) */
  /* { */
  /*   glActiveTexture (GL_TEXTURE0 + i); */
  /*   m_textures[i]->bind(); */
  /*   shader->set_int ("textures[" + std::to_string (i) + "]", i); */
  /* } */
  /* glActiveTexture (GL_TEXTURE0); */

  glBindVertexArray (m_vao);
  glDrawElements (GL_TRIANGLES, m_index_count, GL_UNSIGNED_INT, 0);
  glBindVertexArray (0);
  m_index_count = 0;
}

void BatchQuad::emplace (const std::shared_ptr<Renderable>& renderable, const double x, const double y, const double z)
{
  assert(m_index_count <= INDICES_SIZE);

  const auto quad = std::dynamic_pointer_cast<Quad> (renderable);

  if (quad == nullptr)
  {
    throw std::runtime_error ("Could not cast Renderable to Quad");
  }
  /* const glm::vec2 size                     = sprite->get_size(); */
  /* const std::array<glm::vec2, 4> texcoords = sprite->get_texcoords(); */

  /* const std::shared_ptr<Texture>& texture = sprite->texture; */

  /* assert (sprite->texture != nullptr); */
  /* assert (size.x != 0); */
  /* assert (size.y != 0); */

  /* if (sprite->color) */
  /* { */
  /*   color = sprite->color->int_color; */
  /* } */

  /* // Build vector of textures to bind when rendering */
  /* // texture_index is the index in m_textures that will */
  /* // be translated to a index in the shader. */
  /* int texture_index = 0; */
  /* const auto it     = std::find (m_textures.begin(), m_textures.end(), texture); */
  /* if (it == m_textures.end()) */
  /* { */
  /*   texture_index = m_textures.size(); */
  /*   m_textures.emplace_back (texture); */
  /* } */
  /* else */
  /* { */
  /*   texture_index = it - m_textures.begin(); */
  /* } */

  /* // Get transformations and apply them to the sprite vertices */
  auto general_transform = glm::mat4 (1.0f);
  general_transform = glm::translate (general_transform, glm::vec3 (x, y, z));
  /* if (sprite->transform) */
  /* { */
  /*   // Translate and add pivot for other transformations */
  /*   general_transform = glm::translate (general_transform, */
  /*                                       glm::vec3 (x + sprite->transform->pivot.x, y + sprite->transform->pivot.y, z + sprite->transform->pivot.z)); */

  /*   // Scale only if necessary */
  /*   if (sprite->transform->scale.x != 1.f || sprite->transform->scale.y != 1.f || sprite->transform->scale.z != 1.f) */
  /*   { */
  /*     general_transform = glm::scale (general_transform, sprite->transform->scale); */
  /*   } */

  /*   // Rotate only if necessary */
  /*   if (sprite->transform->rotation.x != 0.0f) */
  /*   { */
  /*     general_transform = glm::rotate (general_transform, glm::radians (sprite->transform->rotation.x), glm::vec3 (1.f, 0.f, 0.f)); */
  /*   } */
  /*   if (sprite->transform->rotation.y != 0.0f) */
  /*   { */
  /*     general_transform = glm::rotate (general_transform, glm::radians (sprite->transform->rotation.y), glm::vec3 (0.f, 1.f, 0.f)); */
  /*   } */
  /*   if (sprite->transform->rotation.z != 0.0f) */
  /*   { */
  /*     general_transform = glm::rotate (general_transform, glm::radians (sprite->transform->rotation.z), glm::vec3 (0.f, 0.f, 1.f)); */
  /*   } */

  /*   // Remove the pivot translation if needed */
  /*   if (sprite->transform->pivot.x != 0.f || sprite->transform->pivot.y != 0.f || sprite->transform->pivot.z != 0.f) */
  /*   { */
  /*     general_transform = glm::translate (general_transform, sprite->transform->pivot * -1.f); */
  /*   } */
  /* } */
  /* else */
  /* { */
  /*   general_transform = glm::translate (general_transform, glm::vec3 (x, y, z)); */
  /* } */

  /* std::cout << "HERE1\n"; */
  // Top left vertex
  glm::vec4 transformation_result = general_transform * glm::vec4 (0.f, 0.f, 1.f, 1.f);
  /* std::cout << "HERE2\n"; */
  m_vertex_buffer->position       = glm::vec3 (transformation_result.x, transformation_result.y, transformation_result.z);
  /* std::cout << "HERE3\n"; */
  m_vertex_buffer->color          = quad->color.int_color;
  /* std::cout << "HERE4\n"; */
  m_vertex_buffer++;
  /* std::cout << "HERE5\n"; */

  // Top right vertex
  transformation_result       = general_transform * glm::vec4 (quad->w, 0.f, 1.f, 1.f);
  m_vertex_buffer->position   = glm::vec3 (transformation_result.x, transformation_result.y, transformation_result.z);
  m_vertex_buffer->color          = quad->color.int_color;
  m_vertex_buffer++;

  // Bottom right vertex
  transformation_result       = general_transform * glm::vec4 (quad->w, quad->h, 1.f, 1.f);
  m_vertex_buffer->position   = glm::vec3 (transformation_result.x, transformation_result.y, transformation_result.z);
  m_vertex_buffer->color          = quad->color.int_color;
  m_vertex_buffer++;

  // Bottom left vertex
  transformation_result       = general_transform * glm::vec4 (0.f, quad->h, 1.f, 1.f);
  m_vertex_buffer->position   = glm::vec3 (transformation_result.x, transformation_result.y, transformation_result.z);
  m_vertex_buffer->color          = quad->color.int_color;
  m_vertex_buffer++;

  // Each quad has 6 vertices, we have therefore to increment by 6 each time
  m_index_count += 6;
}

void BatchQuad::init_emplacing()
{
  glBindBuffer (GL_ARRAY_BUFFER, m_vbo);
  m_vertex_buffer = static_cast<VertexData*> (glMapBuffer (GL_ARRAY_BUFFER, GL_WRITE_ONLY));

  if (m_vertex_buffer == nullptr)
  {
    throw std::runtime_error ("Could not initialize VBO for BatchQuad");
  }
}

void BatchQuad::finalize_emplacing()
{
  glUnmapBuffer (GL_ARRAY_BUFFER);
  glBindBuffer (GL_ARRAY_BUFFER, 0);
}

}
