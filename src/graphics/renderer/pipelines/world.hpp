#pragma once

#include <webgpu/webgpu.h>

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
class Camera;

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
  WGPUTextureView textureView;
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
  WGPUTexture texture;
  WGPUBindGroupLayout bindGroupLayout = nullptr;
  WGPUPipelineLayout pipelineLayout;
  // TEMP

  WGPURenderPipeline pipeline;

  WorldPipeline() = default;
  ~WorldPipeline();

  void load(const WGPUDevice device, const WGPUTextureFormat texture_format, const Shader& shader);
  void render(const WGPURenderPassEncoder render_pass, const Camera& camera);

 private:
  bool m_has_loaded = false;
  WGPUQueue m_queue;
};
}  // namespace dl
