#include "./mesh.hpp"

// TEMP
#include <glm/glm.hpp>
#include <vector>
// TEMP

namespace dl
{
struct VertexData
{
  glm::vec3 position;
  glm::vec2 uv;
  float texture_id;
  uint32_t color;
};

const std::vector<VertexData> vertex_data = {
    // Quad 1
    {
        .position = {0.0f, 0.0f, 0.2f},
        .uv = {0.0f, 0.0f},
        .texture_id = -1.0f,
        .color = 0xff8811ff,
    },
    {
        .position = {100.0f, 100.0f, 0.2f},
        .uv = {1.0f, 1.0f},
        .texture_id = -1.0f,
        .color = 0xff8811ff,
    },
    {
        .position = {0.0f, 100.0f, 0.2f},
        .uv = {0.0f, 1.0f},
        .texture_id = -1.0f,
        .color = 0xff8811ff,
    },
    {
        .position = {0.0f, 0.0f, 0.2f},
        .uv = {0.0f, 0.0f},
        .texture_id = -1.0f,
        .color = 0xff8811ff,
    },
    {
        .position = {100.0f, 0.0f, 0.2f},
        .uv = {1.0f, 0.0f},
        .texture_id = -1.0f,
        .color = 0xff8811ff,
    },
    {
        .position = {100.0f, 100.0f, 0.2f},
        .uv = {1.0f, 1.0f},
        .texture_id = -1.0f,
        .color = 0xff8811ff,
    },

    // Quad 2
    {
        .position = {0.0f, 0.0f, 0.2f},
        .uv = {0.0f, 0.0f},
        .texture_id = 0.0f,
        .color = 0xffffffff,
    },
    {
        .position = {1024.0f, 1024.0f, 0.2f},
        .uv = {1.0f, 1.0f},
        .texture_id = 0.0f,
        .color = 0xffffffff,
    },
    {
        .position = {0.0f, 1024.0f, 0.2f},
        .uv = {0.0f, 1.0f},
        .texture_id = 0.0f,
        .color = 0xffffffff,
    },
    {
        .position = {0.0f, 0.0f, 0.2f},
        .uv = {0.0f, 0.0f},
        .texture_id = 0.0f,
        .color = 0xffffffff,
    },
    {
        .position = {1024.0f, 0.0f, 0.2f},
        .uv = {1.0f, 0.0f},
        .texture_id = 0.0f,
        .color = 0xffffffff,
    },
    {
        .position = {1024.0f, 1024.0f, 0.2f},
        .uv = {1.0f, 1.0f},
        .texture_id = 0.0f,
        .color = 0xffffffff,
    },
};

Mesh::~Mesh()
{
  if (m_has_loaded)
  {
    wgpuBufferDestroy(buffer);
    wgpuBufferRelease(buffer);
  }
}

void Mesh::load(const WGPUDevice device)
{
  WGPUQueue queue = wgpuDeviceGetQueue(device);

  WGPUBufferDescriptor buffer_descriptor = {
      .size = vertex_data.size() * sizeof(VertexData),
      .usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex,
      .mappedAtCreation = false,
  };
  buffer = wgpuDeviceCreateBuffer(device, &buffer_descriptor);

  size = buffer_descriptor.size;
  count = vertex_data.size();

  wgpuQueueWriteBuffer(queue, buffer, 0, vertex_data.data(), size);
  assert(buffer != nullptr);

  m_has_loaded = true;
}
}  // namespace dl
