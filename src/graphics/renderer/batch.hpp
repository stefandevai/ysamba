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

#include "graphics/renderer/vertex_buffer.hpp"
#include "graphics/renderer/vertex_data.hpp"

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
class Texture;

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
  static constexpr uint32_t MAIN_BATCH_VERTEX_COUNT = 80000;
  static constexpr uint32_t SECONDARY_BATCH_VERTEX_COUNT = 2000;
  static constexpr uint32_t TEXTURE_SLOTS = 8;

  Pipeline pipeline{};
  std::vector<VertexBuffer<VertexData>> vertex_buffers{};
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
  VertexBuffer<VertexData>* m_current_vb = nullptr;

  Texture m_dummy_texture;
  uint32_t m_texture_slot_index_base = 0;
  uint32_t m_texture_slot_index = m_texture_slot_index_base;

  void m_load_vertex_buffers();
  void m_load_textures();
};
}  // namespace dl
