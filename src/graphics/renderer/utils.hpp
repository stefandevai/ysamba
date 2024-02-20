#pragma once

#include <webgpu/webgpu.h>

namespace dl::utils
{
WGPUBindGroupLayoutEntry default_binding_layout()
{
  WGPUBindGroupLayoutEntry binding_layout;

  binding_layout.buffer.nextInChain = nullptr;
  // binding_layout.buffer.type = WGPUBufferBindingType_Undefined;
  binding_layout.buffer.hasDynamicOffset = false;

  binding_layout.sampler.nextInChain = nullptr;
  binding_layout.sampler.type = WGPUSamplerBindingType_Undefined;

  binding_layout.storageTexture.nextInChain = nullptr;
  binding_layout.storageTexture.access = WGPUStorageTextureAccess_Undefined;
  binding_layout.storageTexture.format = WGPUTextureFormat_Undefined;
  binding_layout.storageTexture.viewDimension = WGPUTextureViewDimension_Undefined;

  binding_layout.texture.nextInChain = nullptr;
  binding_layout.texture.multisampled = false;
  binding_layout.texture.sampleType = WGPUTextureSampleType_Undefined;
  binding_layout.texture.viewDimension = WGPUTextureViewDimension_Undefined;

  return binding_layout;
}
}  // namespace dl::utils
