/* #include "./batch_quad.hpp" */

/* #include <glad/glad.h> */
/* #include <spdlog/spdlog.h> */

/* #include <glm/gtc/matrix_transform.hpp> */

/* #include "./shader_program.hpp" */
/* #include "./sprite.hpp" */
/* #include "./texture.hpp" */

/* namespace dl */
/* { */
/* BatchQuad::BatchQuad() {} */

/* BatchQuad::BatchQuad(std::shared_ptr<ShaderProgram> shader, const int priority) : Layer(shader, priority) { load(); }
 */

/* BatchQuad::~BatchQuad() */
/* { */
/*   glDeleteBuffers(1, &m_ebo); */
/*   glDeleteBuffers(1, &m_vbo); */
/*   glDeleteVertexArrays(1, &m_vao); */
/* } */

/* void BatchQuad::load() */
/* { */
/*   glGenVertexArrays(1, &m_vao); */
/*   glGenBuffers(1, &m_vbo); */
/*   glGenBuffers(1, &m_ebo); */

/*   glBindVertexArray(m_vao); */
/*   glBindBuffer(GL_ARRAY_BUFFER, m_vbo); */

/*   glBufferData(GL_ARRAY_BUFFER, BUFFER_SIZE, NULL, GL_DYNAMIC_DRAW); */
/*   glVertexAttribPointer(POSITION_INDEX, 3, GL_FLOAT, GL_FALSE, VERTEX_SIZE, (GLvoid*)0); */
/*   glEnableVertexAttribArray(POSITION_INDEX); */

/*   glVertexAttribPointer(COLOR_INDEX, 4, GL_UNSIGNED_BYTE, GL_TRUE, VERTEX_SIZE, (GLvoid*)offsetof(VertexData,
 * color)); */
/*   glEnableVertexAttribArray(COLOR_INDEX); */

/*   GLint offset = 0; */
/*   GLuint indices[INDICES_SIZE]; */
/*   for (GLuint i = 0; i < INDICES_SIZE; i += 6) */
/*   { */
/*     indices[i] = offset; */
/*     indices[i + 1] = offset + 1; */
/*     indices[i + 2] = offset + 2; */

/*     indices[i + 3] = offset; */
/*     indices[i + 4] = offset + 2; */
/*     indices[i + 5] = offset + 3; */

/*     offset += 4; */
/*   } */
/*   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo); */
/*   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); */

/*   glBindVertexArray(0); */
/* } */

/* void BatchQuad::render() */
/* { */
/*   // Return early if no quads were added to the batch */
/*   if (m_index_count == 0) */
/*   { */
/*     return; */
/*   } */

/*   glBindVertexArray(m_vao); */
/*   glDrawElements(GL_TRIANGLES, m_index_count, GL_UNSIGNED_INT, 0); */
/*   glBindVertexArray(0); */
/*   m_index_count = 0; */
/* } */

/* void BatchQuad::emplace(const std::shared_ptr<Renderable>& renderable, const double x, const double y, const double
 * z) */
/* { */
/*   assert(m_index_count <= INDICES_SIZE); */

/*   const auto quad = std::dynamic_pointer_cast<Quad>(renderable); */

/*   if (quad == nullptr) */
/*   { */
/*     throw std::runtime_error("Could not cast Renderable to Quad"); */
/*   } */

/*   /1* // Get transformations and apply them to the sprite vertices *1/ */
/*   auto general_transform = glm::mat4(1.0f); */
/*   general_transform = glm::translate(general_transform, glm::vec3(x, y, z)); */

/*   glm::vec4 transformation_result = general_transform * glm::vec4(0.f, 0.f, 1.f, 1.f); */
/*   m_vertex_buffer->position = glm::vec3(transformation_result.x, transformation_result.y, transformation_result.z);
 */
/*   m_vertex_buffer->color = quad->color.int_color; */
/*   m_vertex_buffer++; */

/*   // Top right vertex */
/*   transformation_result = general_transform * glm::vec4(quad->w, 0.f, 1.f, 1.f); */
/*   m_vertex_buffer->position = glm::vec3(transformation_result.x, transformation_result.y, transformation_result.z);
 */
/*   m_vertex_buffer->color = quad->color.int_color; */
/*   m_vertex_buffer++; */

/*   // Bottom right vertex */
/*   transformation_result = general_transform * glm::vec4(quad->w, quad->h, 1.f, 1.f); */
/*   m_vertex_buffer->position = glm::vec3(transformation_result.x, transformation_result.y, transformation_result.z);
 */
/*   m_vertex_buffer->color = quad->color.int_color; */
/*   m_vertex_buffer++; */

/*   // Bottom left vertex */
/*   transformation_result = general_transform * glm::vec4(0.f, quad->h, 1.f, 1.f); */
/*   m_vertex_buffer->position = glm::vec3(transformation_result.x, transformation_result.y, transformation_result.z);
 */
/*   m_vertex_buffer->color = quad->color.int_color; */
/*   m_vertex_buffer++; */

/*   // Each quad has 6 vertices, we have therefore to increment by 6 each time */
/*   m_index_count += 6; */
/* } */

/* void BatchQuad::init_emplacing() */
/* { */
/*   glBindBuffer(GL_ARRAY_BUFFER, m_vbo); */
/*   m_vertex_buffer = static_cast<VertexData*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY)); */

/*   if (m_vertex_buffer == nullptr) */
/*   { */
/*     throw std::runtime_error("Could not initialize VBO for BatchQuad"); */
/*   } */
/* } */

/* void BatchQuad::finalize_emplacing() */
/* { */
/*   glUnmapBuffer(GL_ARRAY_BUFFER); */
/*   glBindBuffer(GL_ARRAY_BUFFER, 0); */
/* } */
/* }  // namespace dl */
