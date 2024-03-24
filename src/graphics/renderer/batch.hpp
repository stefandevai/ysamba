#pragma once

#include <webgpu/wgpu.h>

#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "core/maths/vector.hpp"
#include "graphics/render_face.hpp"
#include "graphics/renderer/batch_data.hpp"
#include "graphics/renderer/shader.hpp"
#include "graphics/renderer/texture.hpp"
#include "graphics/renderer/vertex_data.hpp"
#include "graphics/renderer/wgpu_context.hpp"

namespace dl
{
struct GameContext;
class Camera;
struct Quad;
class Text;
struct NinePatch;
struct Tile;
struct Sprite;

struct UniformData
{
  uint32_t size = sizeof(glm::mat4) * 2;
  uint32_t projection_matrix_offset = 0;
  uint32_t projection_matrix_size = sizeof(glm::mat4);
  uint32_t view_matrix_offset = sizeof(glm::mat4);
  uint32_t view_matrix_size = sizeof(glm::mat4);
};

struct Pipeline
{
  std::array<WGPUBindGroupLayout, 2> bind_group_layouts{};
  std::array<WGPUBindGroup, 2> bind_groups{};
  WGPUBuffer uniform_buffer{};
  WGPUSampler sampler{};
  UniformData uniform_data{};
  WGPUPipelineLayout layout{};
  WGPURenderPipeline pipeline{};
  bool has_depth_test = true;
};

class Batch
{
 public:
  static constexpr uint32_t MAIN_BATCH_QUAD_COUNT = 80000;
  static constexpr uint32_t MAIN_BATCH_INDEX_COUNT = MAIN_BATCH_QUAD_COUNT * 6;
  static constexpr uint32_t MAIN_BATCH_VERTEX_COUNT = MAIN_BATCH_QUAD_COUNT * 4;

  static constexpr uint32_t SECONDARY_BATCH_QUAD_COUNT = 500;
  static constexpr uint32_t SECONDARY_BATCH_INDEX_COUNT = SECONDARY_BATCH_QUAD_COUNT * 6;
  static constexpr uint32_t SECONDARY_BATCH_VERTEX_COUNT = SECONDARY_BATCH_QUAD_COUNT * 4;

  static constexpr uint32_t TEXTURE_SLOTS = 8;

  Pipeline pipeline{};
  std::vector<BatchData<VertexData>> batch_data{};
  std::array<WGPUTextureView, TEXTURE_SLOTS> texture_views{};
  bool should_update_texture_bind_group = false;

  Batch(GameContext& game_context);

  void load();

  // Returns true if all the vertex buffers are empty
  bool empty();

  // Clear non pinned textures
  void clear_textures();

  // Pin a texture so it can't be cleared and return its slot index
  uint32_t pin_texture(WGPUTextureView texture_view);

  void sprite(
      Sprite& sprite, const double x, const double y, const double z, const RenderFace face = DL_RENDER_FACE_TOP);
  void tile(
      const Tile& tile, const double x, const double y, const double z, const RenderFace face = DL_RENDER_FACE_TOP);
  void quad(const Quad* quad, const double x, const double y, const double z);
  void text(Text& text, const double x, const double y, const double z);
  void nine_patch(NinePatch& nine_patch, const double x, const double y, const double z);

  void push_scissor(Vector4i scissor);
  void pop_scissor();

 private:
  struct SpriteBatchData
  {
    const RenderFace face{};
    const glm::vec3& position;
    const glm::vec2& size;
    const std::array<glm::vec2, 4>& texture_coordinates;
    uint32_t color{};
    float texture_index{};
  };

  GameContext& m_game_context;
  WGPUContext& m_context;
  BatchData<VertexData>* m_current_vb = nullptr;

  Texture m_dummy_texture;
  uint32_t m_texture_slot_index_base = 0;
  uint32_t m_texture_slot_index = m_texture_slot_index_base;

  void m_load_batch_data();
  void m_load_textures();
  void m_emplace_sprite_face(const SpriteBatchData data);
  void m_emplace_sprite_face_top(const SpriteBatchData data);
  void m_emplace_sprite_face_front(const SpriteBatchData data);
};
}  // namespace dl
