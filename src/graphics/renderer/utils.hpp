#pragma once

#include <webgpu/wgpu.h>

namespace dl::utils
{
WGPUStencilFaceState default_stencil_face_state()
{
  WGPUStencilFaceState stencil_face_state = {
      .compare = WGPUCompareFunction_Always,
      .failOp = WGPUStencilOperation_Keep,
      .depthFailOp = WGPUStencilOperation_Keep,
      .passOp = WGPUStencilOperation_Keep,
  };

  return stencil_face_state;
}

WGPUDepthStencilState default_depth_stencil_state()
{
  WGPUDepthStencilState depth_stencil_state = {
      .format = WGPUTextureFormat_Undefined,
      .depthWriteEnabled = false,
      .depthCompare = WGPUCompareFunction_Always,
      .stencilReadMask = 0xFFFFFFFF,
      .stencilWriteMask = 0xFFFFFFFF,
      .depthBias = 0,
      .depthBiasSlopeScale = 0,
      .depthBiasClamp = 0,
      .stencilFront = default_stencil_face_state(),
      .stencilBack = default_stencil_face_state(),
  };

  return depth_stencil_state;
}

}  // namespace dl::utils
