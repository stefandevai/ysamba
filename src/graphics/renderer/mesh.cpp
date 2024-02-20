#include "./mesh.hpp"

// TEMP
#include <vector>
// TEMP

namespace dl
{
const std::vector<float> vertex_data{
    // First quad
    -0.5f,
    0.5f,
    0.5f,
    0.9f,
    0.6f,
    0.3f,
    -0.5f,
    -0.5f,
    0.5f,
    0.9f,
    0.6f,
    0.3f,
    0.5f,
    -0.5f,
    0.5f,
    0.9f,
    0.6f,
    0.3f,
    -0.5f,
    0.5f,
    0.5f,
    0.9f,
    0.6f,
    0.3f,
    0.5f,
    0.5f,
    0.5f,
    0.9f,
    0.6f,
    0.3f,
    0.5f,
    -0.5f,
    0.5f,
    0.9f,
    0.6f,
    0.3f,

    // Second quad
    -0.3f,
    0.7f,
    0.6f,
    0.3f,
    0.6f,
    0.9f,
    -0.3f,
    -0.3f,
    0.6f,
    0.3f,
    0.6f,
    0.9f,
    0.7f,
    -0.3f,
    0.6f,
    0.3f,
    0.6f,
    0.9f,
    -0.3f,
    0.7f,
    0.6f,
    0.3f,
    0.6f,
    0.9f,
    0.7f,
    0.7f,
    0.6f,
    0.3f,
    0.6f,
    0.9f,
    0.7f,
    -0.3f,
    0.6f,
    0.3f,
    0.6f,
    0.9f,
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

  WGPUBufferDescriptor buffer_descriptor = {};
  buffer_descriptor.nextInChain = nullptr;
  buffer_descriptor.size = vertex_data.size() * sizeof(float);
  buffer_descriptor.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex;
  buffer_descriptor.mappedAtCreation = false;
  buffer = wgpuDeviceCreateBuffer(device, &buffer_descriptor);

  size = buffer_descriptor.size;
  count = vertex_data.size();

  wgpuQueueWriteBuffer(queue, buffer, 0, vertex_data.data(), size);
  m_has_loaded = true;
}
}  // namespace dl
