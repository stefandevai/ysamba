#pragma once

#include <webgpu/wgpu.h>

#include <array>
#include <vector>

#include "core/maths/vector.hpp"
#include "graphics/renderer/shader.hpp"
#include "graphics/renderer/texture.hpp"
#include "graphics/renderer/wgpu_context.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "graphics/renderer/mesh.hpp"
#include "graphics/renderer/vertex_buffer.hpp"

namespace dl
{
struct GameContext;
class Camera;
class Sprite;
struct Quad;
class Text;
struct NinePatch;
struct TileRenderData;
class MultiSprite;

namespace v2
{
class Texture;

class WorldPipeline
{
 public:
  // TEMP
  Mesh mesh{};
  uint32_t index_count = 0;
  uint32_t vertex_size;
  uint32_t vertex_count;

  std::array<WGPUBindGroupLayoutEntry, 2> binding_layout{};
  std::array<WGPUBindGroupEntry, 2> binding{};
  std::array<WGPUBindGroupLayout, 2> bindGroupLayouts{};
  WGPUBindGroup bindGroup{};

  WGPUBindGroupLayoutEntry texture_binding_layout{};
  WGPUBindGroupEntry texture_binding{};
  WGPUBindGroup texture_bind_group{};

  WGPUBuffer uniformBuffer;
  WGPUDepthStencilState stencil_state;
  WGPUSampler sampler;

  struct UniformData
  {
    uint32_t size = sizeof(glm::mat4) * 2;

    uint32_t projection_matrix_offset = 0;
    uint32_t projection_matrix_size = sizeof(glm::mat4);
    uint32_t view_matrix_offset = sizeof(glm::mat4);
    uint32_t view_matrix_size = sizeof(glm::mat4);
  };
  UniformData uniform_data;
  WGPUPipelineLayout pipelineLayout;
  // TEMP

  WGPURenderPipeline pipeline;

  WorldPipeline(GameContext& game_context);
  ~WorldPipeline();

  void load(const Shader& shader);
  void render(const WGPURenderPassEncoder render_pass, const Camera& camera);
  void clear_textures();

  void sprite(Sprite* sprite, const double x, const double y, const double z);
  void multi_sprite(MultiSprite* sprite, const double x, const double y, const double z);
  void tile(const TileRenderData& tile, const double x, const double y, const double z);
  void quad(const Quad* quad, const double x, const double y, const double z);
  void text(Text& text, const double x, const double y, const double z);
  void nine_patch(NinePatch& nine_patch, const double x, const double y, const double z);

  void push_scissor(Vector4i scissor);
  void pop_scissor();

 private:
  GameContext& m_game_context;
  WGPUContext& m_context;
  bool m_should_update_texture_bind_group = false;
  bool m_has_loaded = false;

  struct VertexData
  {
    glm::vec3 position;
    glm::vec2 texture_coordinates;
    float texture_id;
    uint32_t color;
  };

  static constexpr uint32_t m_max_quads = 80000;
  static constexpr uint32_t m_vertex_size = sizeof(VertexData);
  static constexpr uint32_t m_quad_size = 4 * m_vertex_size;
  static constexpr uint32_t m_buffer_size = m_max_quads * m_quad_size;
  static constexpr uint32_t m_indices_size = 6 * m_max_quads;
  static constexpr uint32_t TEXTURE_SLOTS = 8;

  std::vector<VertexData> m_vertices{};
  uint32_t m_vertices_index = 0;
  WGPUBuffer m_vertex_buffer{};

  // VertexBuffer<VertexData> m_main_vb;
  // std::vector<VertexBuffer<VertexData>> m_secondary_vbs{};
  std::vector<VertexBuffer<VertexData>> m_vertex_buffers{};
  VertexBuffer<VertexData>* m_current_vb = nullptr;

  Texture m_dummy_texture;
  std::array<WGPUTextureView, TEXTURE_SLOTS> m_texture_views{};
  uint32_t m_texture_slot_index = 0;

  void m_update_texture_bind_group();
};
}  // namespace v2
}  // namespace dl
