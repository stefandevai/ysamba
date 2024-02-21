#pragma once

#include <webgpu/wgpu.h>

namespace dl
{
class Mesh
{
 public:
  WGPUBuffer buffer;
  size_t size;
  uint32_t count;

  Mesh() = default;
  ~Mesh();

  void load(const WGPUDevice device);

 private:
  bool m_has_loaded = false;
};
}  // namespace dl
