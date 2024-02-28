#pragma once

#include <webgpu/wgpu.h>

#include "core/maths/vector.hpp"

namespace dl
{
template <typename T>
struct VertexBuffer
{
  WGPUBuffer buffer;
  WGPUBuffer index_buffer;
  uint32_t index = 0;
  uint32_t max_vertex_size = 0;
  uint32_t max_index_size = 0;
  uint32_t size = 0;
  std::vector<T> vertices{};
  std::vector<uint32_t> indices{};
  Vector4i scissor{0, 0, -1, -1};

  // Constructor
  VertexBuffer(WGPUDevice device, const uint32_t max_vertex_size, const uint32_t max_index_size)
      : max_vertex_size(max_vertex_size), max_index_size(max_index_size)
  {
    // Create vertex buffer
    vertices.resize(max_vertex_size);
    WGPUBufferDescriptor buffer_descriptor = {
        .size = max_vertex_size * sizeof(T),
        .usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex,
        .mappedAtCreation = false,
    };
    buffer = wgpuDeviceCreateBuffer(device, &buffer_descriptor);
    assert(buffer != nullptr);

    // Create index buffer
    indices.resize(max_index_size);
    WGPUBufferDescriptor index_buffer_descriptor = {
        .size = max_index_size * sizeof(uint32_t),
        .usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Index,
        .mappedAtCreation = false,
    };
    index_buffer = wgpuDeviceCreateBuffer(device, &index_buffer_descriptor);
    assert(index_buffer != nullptr);
  }

  // Destructor
  ~VertexBuffer()
  {
    if (index_buffer != nullptr)
    {
      wgpuBufferDestroy(index_buffer);
      wgpuBufferRelease(index_buffer);
    }
    if (buffer != nullptr)
    {
      wgpuBufferDestroy(buffer);
      wgpuBufferRelease(buffer);
    }
  }

  // Delete copy assignment operator and copy constructor
  VertexBuffer& operator=(const VertexBuffer& rhs) = delete;
  VertexBuffer(const VertexBuffer& rhs) = delete;

  // Move assignment operator and move constructor
  VertexBuffer&& operator=(VertexBuffer&& rhs) noexcept
  {
    buffer = rhs.buffer;
    index_buffer = rhs.index_buffer;
    index = rhs.index;
    size = rhs.size;
    max_vertex_size = rhs.max_vertex_size;
    max_index_size = rhs.max_index_size;
    vertices = std::move(rhs.vertices);
    scissor = std::move(rhs.scissor);

    rhs.buffer = nullptr;
    rhs.index = 0;
    rhs.size = 0;
  }

  VertexBuffer(VertexBuffer&& rhs) noexcept
  {
    buffer = rhs.buffer;
    index_buffer = rhs.index_buffer;
    index = rhs.index;
    size = rhs.size;
    max_vertex_size = rhs.max_vertex_size;
    max_index_size = rhs.max_index_size;
    vertices = std::move(rhs.vertices);
    scissor = std::move(rhs.scissor);

    rhs.buffer = nullptr;
    rhs.index = 0;
    rhs.size = 0;
  }

  template <typename... Args>
  void emplace(Args&&... args)
  {
    assert(index < max_vertex_size);
    vertices[index++] = T{std::forward<Args>(args)...};
  }

  void update(WGPUQueue queue)
  {
    size = index * sizeof(T);
    wgpuQueueWriteBuffer(queue, buffer, 0, vertices.data(), size);
  }

  void reset()
  {
    index = 0;
    size = 0;
    scissor = {0, 0, -1, -1};
  }

  bool has_scissor() { return scissor.z > -1 && scissor.w > -1; }
};
}  // namespace dl
