#pragma once

#include <glm/glm.hpp>
#include <entt/entity/registry.hpp>

namespace dl
{
typedef unsigned int GLuint;
typedef float GLfloat;
typedef int GLsizei;

class Texture;
class Sprite;
class ShaderProgram;

class Batch2D
{
public:
  std::shared_ptr<ShaderProgram> shader;

  Batch2D(std::shared_ptr<ShaderProgram> shader);
  ~Batch2D();

  void render ();
  void emplace (entt::registry& registry, entt::entity entity);
  void emplace (const std::shared_ptr<Sprite>& sprite, const double x, const double y, const double z);
  void init_emplacing();
  void finalize_emplacing();

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
    POSITION_INDEX,
    TEXCOORDS_INDEX,
    TEXTURE_ID_INDEX,
    COLOR_INDEX
  };

  static const unsigned MAX_SPRITES  = 10000;
  static const unsigned VERTEX_SIZE  = sizeof (VertexData);
  static const unsigned SPRITE_SIZE  = 4 * VERTEX_SIZE;
  static const unsigned BUFFER_SIZE  = MAX_SPRITES * SPRITE_SIZE;
  static const unsigned INDICES_SIZE = 6 * MAX_SPRITES;

  VertexData* m_vertex_buffer = nullptr; // Needed to write data to the mapped vertex buffer
  unsigned int m_vao;
  unsigned int m_vbo;
  unsigned int m_ebo;
  unsigned int m_index_count = 0;
  std::vector<std::shared_ptr<Texture>> m_textures;
};
}
