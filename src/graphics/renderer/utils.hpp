#pragma once

#include <webgpu/webgpu.h>

namespace dl::utils
{
WGPUBindGroupLayoutEntry default_binding_layout()
{
  WGPUBindGroupLayoutEntry binding_layout{};

  binding_layout.buffer.nextInChain = nullptr;
  binding_layout.buffer.type = WGPUBufferBindingType_Undefined;
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

WGPUStencilFaceState default_stencil_face_state()
{
  WGPUStencilFaceState stencil_face_state;

  stencil_face_state.compare = WGPUCompareFunction_Always;
  stencil_face_state.failOp = WGPUStencilOperation_Keep;
  stencil_face_state.depthFailOp = WGPUStencilOperation_Keep;
  stencil_face_state.passOp = WGPUStencilOperation_Keep;

  return stencil_face_state;
}

WGPUDepthStencilState default_depth_stencil_state()
{
  WGPUDepthStencilState depth_stencil_state;

  depth_stencil_state.format = WGPUTextureFormat_Undefined;
  depth_stencil_state.depthWriteEnabled = false;
  depth_stencil_state.depthCompare = WGPUCompareFunction_Always;
  depth_stencil_state.stencilReadMask = 0xFFFFFFFF;
  depth_stencil_state.stencilWriteMask = 0xFFFFFFFF;
  depth_stencil_state.depthBias = 0;
  depth_stencil_state.depthBiasSlopeScale = 0;
  depth_stencil_state.depthBiasClamp = 0;
  depth_stencil_state.stencilFront = default_stencil_face_state();
  depth_stencil_state.stencilBack = default_stencil_face_state();

  return depth_stencil_state;
}

}  // namespace dl::utils
