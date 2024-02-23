#pragma once

#include <webgpu/wgpu.h>

#include "core/maths/vector.hpp"

namespace dl
{
template <typename T>
struct VertexBuffer
{
  WGPUBuffer buffer;
  uint32_t index = 0;
  uint32_t max_size = 0;
  uint32_t size = 0;
  std::vector<T> vertices{};
  Vector4i scissor{0, 0, -1, -1};

  // Constructor
  VertexBuffer(WGPUDevice device, const uint32_t max_size) : max_size(max_size)
  {
    vertices.resize(max_size);
    WGPUBufferDescriptor buffer_descriptor = {
        .size = max_size * sizeof(T),
        .usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex,
        .mappedAtCreation = false,
    };
    buffer = wgpuDeviceCreateBuffer(device, &buffer_descriptor);
    assert(buffer != nullptr);
  }

  // Destructor
  ~VertexBuffer()
  {
    if (buffer == nullptr)
    {
      return;
    }

    wgpuBufferDestroy(buffer);
    wgpuBufferRelease(buffer);
  }

  // Delete copy assignment operator and copy constructor
  VertexBuffer& operator=(const VertexBuffer& rhs) = delete;
  VertexBuffer(const VertexBuffer& rhs) = delete;

  // Move assignment operator and move constructor
  VertexBuffer&& operator=(VertexBuffer&& rhs) noexcept
  {
    buffer = rhs.buffer;
    index = rhs.index;
    size = rhs.size;
    max_size = rhs.max_size;
    vertices = std::move(rhs.vertices);
    scissor = std::move(rhs.scissor);

    rhs.buffer = nullptr;
    rhs.index = 0;
    rhs.size = 0;
  }

  VertexBuffer(VertexBuffer&& rhs) noexcept
  {
    buffer = rhs.buffer;
    index = rhs.index;
    size = rhs.size;
    max_size = rhs.max_size;
    vertices = std::move(rhs.vertices);
    scissor = std::move(rhs.scissor);

    rhs.buffer = nullptr;
    rhs.index = 0;
    rhs.size = 0;
  }

  template <typename... Args>
  void emplace(Args&&... args)
  {
    assert(index < max_size);
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
