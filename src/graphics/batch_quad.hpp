#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "./layer.hpp"
#include "./quad.hpp"

namespace dl
{
typedef unsigned int GLuint;
typedef float GLfloat;
typedef int GLsizei;

class ShaderProgram;

class BatchQuad : public Layer
{
 public:
  BatchQuad(std::shared_ptr<ShaderProgram> shader);
  ~BatchQuad();

  void render();
  void emplace(const std::shared_ptr<Renderable>& renderable, const double x, const double y, const double z);
  void init_emplacing();
  void finalize_emplacing();
  bool get_should_render() { return m_index_count > 0; }

 private:
  using Layer::emplace;

  struct VertexData
  {
    glm::vec3 position;
    unsigned int color;
  };

  enum Index
  {
    POSITION_INDEX,
    COLOR_INDEX
  };

  static const unsigned MAX_QUADS = 10000;
  static const unsigned VERTEX_SIZE = sizeof(VertexData);
  static const unsigned QUAD_SIZE = 4 * VERTEX_SIZE;
  static const unsigned BUFFER_SIZE = MAX_QUADS * QUAD_SIZE;
  static const unsigned INDICES_SIZE = 6 * MAX_QUADS;

  VertexData* m_vertex_buffer = nullptr;  // Needed to write data to the mapped vertex buffer
  unsigned int m_vao;
  unsigned int m_vbo;
  unsigned int m_ebo;
  unsigned int m_index_count = 0;
};
}  // namespace dl
