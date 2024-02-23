#pragma once

#include <webgpu/wgpu.h>

namespace dl::utils
{
WGPUAdapter request_adapter(WGPUInstance instance, WGPURequestAdapterOptions const* options);

WGPUAdapterProperties adapter_properties(WGPUAdapter adapter);

WGPUDevice request_device(WGPUAdapter adapter, WGPUDeviceDescriptor const* descriptor);

void list_features(WGPUAdapter adapter);

void device_set_uncaptured_error_callback(WGPUDevice device);

void queue_on_submitted_work_done(WGPUQueue queue);

WGPUStencilFaceState default_stencil_face_state();

WGPUDepthStencilState default_depth_stencil_state();

}  // namespace dl::utils
