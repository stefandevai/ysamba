#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace dl
{
typedef unsigned int GLuint;
typedef float GLfloat;
typedef int GLsizei;

class ShaderProgram;
class Texture;
class Sprite;
struct Quad;

class Batch
{
 public:
  std::shared_ptr<ShaderProgram> shader = nullptr;
  int priority = 0;

  Batch(const int priority = 0);
  Batch(std::shared_ptr<ShaderProgram> shader, const int priority = 0);
  ~Batch();

  void load();
  void render();
  void emplace(const std::shared_ptr<Sprite>& renderable, const double x, const double y, const double z);
  void quad(const std::shared_ptr<Quad>& renderable, const double x, const double y, const double z);
  bool get_should_render() { return m_index_count > 0; }
  bool get_ignore_camera() const { return m_ignore_camera; }
  void set_ignore_camera(const bool ignore_camera) { m_ignore_camera = ignore_camera; }

 private:
  struct VertexData
  {
    glm::vec3 position;
    glm::vec2 texcoords;
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
  unsigned int m_vao;
  unsigned int m_vbo;
  unsigned int m_ebo;
  unsigned int m_index_count = 0;
  unsigned int m_vertices_index = 0;
  std::vector<std::shared_ptr<Texture>> m_textures;
  bool m_ignore_camera = false;
};
}  // namespace dl
