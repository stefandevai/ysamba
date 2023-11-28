#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "./layer.hpp"

namespace dl
{
typedef unsigned int GLuint;
typedef float GLfloat;
typedef int GLsizei;

class Texture;
class Sprite;
class ShaderProgram;

class Batch : public Layer
{
 public:
  Batch();
  Batch(std::shared_ptr<ShaderProgram> shader, const int priority = 0);
  ~Batch();

  void load();
  void render();
  void emplace(const std::shared_ptr<Renderable>& renderable, const double x, const double y, const double z);
  void quad(const std::shared_ptr<Renderable>& renderable, const double x, const double y, const double z);
  bool get_should_render() { return m_index_count > 0; }

 private:
  using Layer::emplace;

  struct VertexData
  {
    glm::vec3 position;
    glm::vec2 texcoords;
    GLfloat texture_id;
    unsigned int color;
  };

  enum Index
  {
    POSITION_INDEX,
    TEXCOORDS_INDEX,
    TEXTURE_ID_INDEX,
    COLOR_INDEX
  };

  static constexpr unsigned MAX_SPRITES = 10000;
  static constexpr unsigned VERTEX_SIZE = sizeof(VertexData);
  static constexpr unsigned SPRITE_SIZE = 4 * VERTEX_SIZE;
  static constexpr unsigned BUFFER_SIZE = MAX_SPRITES * SPRITE_SIZE;
  static constexpr unsigned INDICES_SIZE = 6 * MAX_SPRITES;

  VertexData* m_vertex_buffer = nullptr;  // Needed to write data to the mapped vertex buffer
  std::vector<VertexData> m_vertices;  // Needed to write data to the mapped vertex buffer
  unsigned int m_vao;
  unsigned int m_vbo;
  unsigned int m_ebo;
  unsigned int m_index_count = 0;
  unsigned int m_vertices_index = 0;
  std::vector<std::shared_ptr<Texture>> m_textures;
};
}  // namespace dl
