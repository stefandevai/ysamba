#pragma once

#include <webgpu/wgpu.h>

namespace dl
{
struct WGPUContext
{
  WGPUInstance instance;
  WGPUAdapter adapter;
  WGPUDevice device;
  WGPUQueue queue;
  WGPUSurface surface;
  WGPUTextureFormat surface_format;
};
}  // namespace dl
