#pragma once

#include <webgpu/wgpu.h>

#include "core/maths/vector.hpp"

namespace dl
{
template <typename T>
struct BatchData
{
  WGPUBuffer vertex_buffer;
  WGPUBuffer index_buffer;
  uint32_t vertex_buffer_count = 0;
  uint32_t index_buffer_count = 0;
  uint32_t max_vertex_size = 0;
  uint32_t max_index_size = 0;
  uint32_t vertex_buffer_size = 0;
  uint32_t index_buffer_size = 0;
  std::vector<T> vertices{};
  std::vector<uint32_t> indices{};
  Vector4i scissor{0, 0, -1, -1};

  // Constructor
  BatchData(WGPUDevice device, const uint32_t max_vertex_size, const uint32_t max_index_size)
      : max_vertex_size(max_vertex_size), max_index_size(max_index_size)
  {
    // Create vertex buffer
    vertices.resize(max_vertex_size);
    WGPUBufferDescriptor buffer_descriptor = {
        .size = max_vertex_size * sizeof(T),
        .usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex,
        .mappedAtCreation = false,
    };
    vertex_buffer = wgpuDeviceCreateBuffer(device, &buffer_descriptor);
    assert(vertex_buffer != nullptr);

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
  ~BatchData()
  {
    if (index_buffer != nullptr)
    {
      wgpuBufferDestroy(index_buffer);
      wgpuBufferRelease(index_buffer);
    }
    if (vertex_buffer != nullptr)
    {
      wgpuBufferDestroy(vertex_buffer);
      wgpuBufferRelease(vertex_buffer);
    }
  }

  // Delete copy assignment operator and copy constructor
  BatchData& operator=(const BatchData& rhs) = delete;
  BatchData(const BatchData& rhs) = delete;

  // Move assignment operator and move constructor
  BatchData&& operator=(BatchData&& rhs) noexcept
  {
    vertex_buffer = rhs.vertex_buffer;
    vertex_buffer_count = rhs.vertex_buffer_count;
    vertex_buffer_size = rhs.vertex_buffer_size;
    index_buffer = rhs.index_buffer;
    index_buffer_count = rhs.index_buffer_count;
    index_buffer_size = rhs.index_buffer_size;
    max_vertex_size = rhs.max_vertex_size;
    max_index_size = rhs.max_index_size;
    vertices = std::move(rhs.vertices);
    scissor = std::move(rhs.scissor);

    rhs.vertex_buffer = nullptr;
    rhs.index_buffer = nullptr;
    rhs.vertex_buffer_count = 0;
    rhs.vertex_buffer_size = 0;
    rhs.index_buffer_count = 0;
    rhs.index_buffer_size = 0;
  }

  BatchData(BatchData&& rhs) noexcept
  {
    vertex_buffer = rhs.vertex_buffer;
    vertex_buffer_count = rhs.vertex_buffer_count;
    vertex_buffer_size = rhs.vertex_buffer_size;
    index_buffer = rhs.index_buffer;
    index_buffer_count = rhs.index_buffer_count;
    index_buffer_size = rhs.index_buffer_size;
    max_vertex_size = rhs.max_vertex_size;
    max_index_size = rhs.max_index_size;
    vertices = std::move(rhs.vertices);
    scissor = std::move(rhs.scissor);

    rhs.vertex_buffer = nullptr;
    rhs.index_buffer = nullptr;
    rhs.vertex_buffer_count = 0;
    rhs.vertex_buffer_size = 0;
    rhs.index_buffer_count = 0;
    rhs.index_buffer_size = 0;
  }

  template <typename... Args>
  void emplace(Args&&... args)
  {
    assert(vertex_buffer_count < max_vertex_size);
    vertices[vertex_buffer_count++] = T{std::forward<Args>(args)...};
  }

  void update(WGPUQueue queue)
  {
    vertex_buffer_size = vertex_buffer_count * sizeof(T);
    index_buffer_size = index_buffer_count * sizeof(uint32_t);
    wgpuQueueWriteBuffer(queue, vertex_buffer, 0, vertices.data(), vertex_buffer_size);
  }

  void reset()
  {
    vertex_buffer_count = 0;
    index_buffer_count = 0;
    vertex_buffer_size = 0;
    index_buffer_size = 0;
    scissor = {0, 0, -1, -1};
  }

  bool has_scissor() { return scissor.z > -1 && scissor.w > -1; }
};
}  // namespace dl
