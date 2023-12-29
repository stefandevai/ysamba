#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <string>
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
struct NinePatch;
struct Tile;

class Batch
{
 public:
  std::shared_ptr<ShaderProgram> shader = nullptr;
  std::string shader_id{};
  uint32_t index_count = 0;
  int priority = 0;
  bool has_scissor = false;
  bool has_depth = true;
  bool has_blend = true;
  Vector4i scissor{0, 0, -1, -1};

  Batch(const int priority = 0);
  Batch(const std::string& shader_id, const int priority = 0);
  ~Batch();

  void load();
  void render();
  void push_matrix(const glm::mat4& matrix);
  const glm::mat4 pop_matrix();
  const glm::mat4& peek_matrix();
  void emplace(Sprite* sprite, const double x, const double y, const double z);
  void emplace(const MultiSprite* sprite, const double x, const double y, const double z);
  void tile(const Tile& tile, const double x, const double y, const double z);
  void quad(const Quad* quad, const double x, const double y, const double z);
  void text(Text& text, const double x, const double y, const double z);
  void nine_patch(NinePatch& nine_patch, const double x, const double y, const double z);
  void add_scissor(const Vector4i& scissor);

 private:
  struct VertexData
  {
    glm::vec3 position;
    glm::vec2 texture_coordinates;
    float texture_id;
    uint32_t color;
  };

  enum Index
  {
    PositionIndex,
    TextureCoordsIndex,
    TextureIdIndex,
    ColorIndex,
  };

  static constexpr uint32_t m_max_quads = 40000;
  static constexpr uint32_t m_vertex_size = sizeof(VertexData);
  static constexpr uint32_t m_quad_size = 4 * m_vertex_size;
  static constexpr uint32_t m_buffer_size = m_max_quads * m_quad_size;
  static constexpr uint32_t m_indices_size = 6 * m_max_quads;

  std::vector<VertexData> m_vertices{};
  std::vector<glm::mat4> m_matrix_stack{};
  glm::mat4 m_matrix = glm::mat4{1.f};
  uint32_t m_vao;
  uint32_t m_vbo;
  uint32_t m_ebo;
  uint32_t m_vertices_index = 0;
  std::vector<Texture*> m_textures;
};
}  // namespace dl
