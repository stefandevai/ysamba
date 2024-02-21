#pragma once

#include <webgpu/wgpu.h>

#include <array>

#include "graphics/renderer/shader.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "graphics/renderer/mesh.hpp"

namespace dl
{
struct WGPUContext;
class Camera;
class Sprite;
struct Quad;
class Text;

namespace v2
{
class WorldPipeline
{
 public:
  // TEMP
  Mesh mesh{};
  uint32_t vertex_size;
  uint32_t vertex_count;
  std::array<WGPUBindGroupLayoutEntry, 3> binding_layout{};
  std::array<WGPUBindGroupEntry, 3> binding{};
  WGPUBindGroup bindGroup;
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
  WGPUBindGroupLayout bindGroupLayout = nullptr;
  WGPUPipelineLayout pipelineLayout;
  // TEMP

  WGPURenderPipeline pipeline;

  WorldPipeline(WGPUContext& context);
  ~WorldPipeline();

  void load(const Shader& shader);
  void render(const WGPURenderPassEncoder render_pass, const Camera& camera);

  void sprite(Sprite* sprite, const double x, const double y, const double z);
  void quad(const Quad* quad, const double x, const double y, const double z);
  void text(Text& text, const double x, const double y, const double z);

 private:
  WGPUContext& m_context;
  bool m_has_loaded = false;
};
}  // namespace v2
}  // namespace dl
