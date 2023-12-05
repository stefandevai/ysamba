#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <vector>

#include "core/maths/vector.hpp"

namespace dl
{
typedef unsigned int GLuint;
typedef float GLfloat;
typedef int GLsizei;

class ShaderProgram;
class Texture;
class Sprite;
class MultiSprite;
struct Quad;
class Text;

class Batch
{
 public:
  std::shared_ptr<ShaderProgram> shader = nullptr;
  int priority = 0;
  bool has_scissor = false;
  bool has_depth = true;
  Vector4i scissor{0, 0, -1, -1};

  Batch(const int priority = 0);
  Batch(std::shared_ptr<ShaderProgram> shader, const int priority = 0);
  ~Batch();

  void load();
  void render();
  void push_matrix(const glm::mat4& matrix);
  const glm::mat4 pop_matrix();
  const glm::mat4& peek_matrix();
  void emplace(const std::shared_ptr<Sprite>& sprite, const double x, const double y, const double z);
  void emplace(const std::shared_ptr<MultiSprite>& sprite, const double x, const double y, const double z);
  void quad(const std::shared_ptr<Quad>& quad, const double x, const double y, const double z);
  void text(Text& text, const double x, const double y, const double z);
  bool get_should_render() { return m_index_count > 0; }
  void add_scissor(const Vector4i& scissor);

 private:
  struct VertexData
  {
    glm::vec3 position;
    glm::vec2 texture_coordinates;
    GLfloat texture_id;
    unsigned int color;
  };

  enum Index
  {
    PositionIndex,
    TextureCoordsIndex,
    TextureIdIndex,
    ColorIndex,
  };

  static constexpr unsigned m_max_quads = 10000;
  static constexpr unsigned m_vertex_size = sizeof(VertexData);
  static constexpr unsigned m_quad_size = 4 * m_vertex_size;
  static constexpr unsigned m_buffer_size = m_max_quads * m_quad_size;
  static constexpr unsigned m_indices_size = 6 * m_max_quads;

  std::vector<VertexData> m_vertices;
  std::vector<glm::mat4> m_matrix_stack;
  glm::mat4 m_matrix = glm::mat4{1.f};
  unsigned int m_vao;
  unsigned int m_vbo;
  unsigned int m_ebo;
  unsigned int m_index_count = 0;
  unsigned int m_vertices_index = 0;
  std::vector<std::shared_ptr<Texture>> m_textures;
  /* bool m_ignore_camera = false; */
};
}  // namespace dl
