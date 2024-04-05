#include "./utils.hpp"

#include <spdlog/spdlog.h>

namespace dl::utils
{
WGPUAdapter request_adapter(WGPUInstance instance, WGPURequestAdapterOptions const* options)
{
  struct UserData
  {
    WGPUAdapter adapter = nullptr;
    bool request_ended = false;
  };
  UserData user_data;

  auto on_adapter_request_ended
      = [](WGPURequestAdapterStatus status, WGPUAdapter adapter, char const* message, void* user_data_)
  {
    UserData& user_data = *reinterpret_cast<UserData*>(user_data_);

    if (status == WGPURequestAdapterStatus_Success && adapter)
    {
      user_data.adapter = adapter;
    }
    else
    {
      spdlog::critical("Failed to request WebGPU adapter: ", message);
    }

    user_data.request_ended = true;
  };

  wgpuInstanceRequestAdapter(instance, options, on_adapter_request_ended, (void*)&user_data);

  assert(user_data.request_ended);

  return user_data.adapter;
}

WGPUDevice request_device(WGPUAdapter adapter, WGPUDeviceDescriptor const* descriptor)
{
  struct UserData
  {
    WGPUDevice device = nullptr;
    bool request_ended = false;
  };
  UserData user_data;

  auto on_device_request_ended
      = [](WGPURequestDeviceStatus status, WGPUDevice device, char const* message, void* user_data_)
  {
    UserData& user_data = *reinterpret_cast<UserData*>(user_data_);

    if (status == WGPURequestDeviceStatus_Success && device)
    {
      user_data.device = device;
    }
    else
    {
      spdlog::critical("Failed to request WebGPU device: ", message);
    }

    user_data.request_ended = true;
  };

  wgpuAdapterRequestDevice(adapter, descriptor, on_device_request_ended, (void*)&user_data);

  assert(user_data.request_ended);

  return user_data.device;
}

void device_set_uncaptured_error_callback(WGPUDevice device)
{
  auto on_device_error = [](WGPUErrorType type, const char* message, void*)
  { spdlog::critical("Device error: {}, {}", (uint32_t)type, message); };
  wgpuDeviceSetUncapturedErrorCallback(device, on_device_error, nullptr);
}

void queue_on_submitted_work_done(WGPUQueue queue)
{
  auto on_queue_work_done
      = [](WGPUQueueWorkDoneStatus status, void*) { spdlog::debug("Queue work done. Status: {}", (uint32_t)status); };

  wgpuQueueOnSubmittedWorkDone(queue, on_queue_work_done, nullptr);
}

void list_features(WGPUAdapter adapter)
{
  size_t featureCount = wgpuAdapterEnumerateFeatures(adapter, nullptr);
  std::vector<WGPUFeatureName> features(featureCount);
  wgpuAdapterEnumerateFeatures(adapter, features.data());

  for (auto f : features)
  {
    spdlog::debug("Feature: 0x{0:x}", (uint64_t)f);
  }
}
WGPUAdapterProperties adapter_properties(WGPUAdapter adapter)
{
  WGPUAdapterProperties properties = {
      .nextInChain = nullptr,
  };
  wgpuAdapterGetProperties(adapter, &properties);
  return properties;
}

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

void populate_quad_index_buffer(WGPUQueue queue, WGPUBuffer buffer, uint32_t index_count)
{
  uint32_t offset = 0;
  uint32_t count = 0;
  uint32_t indices[index_count];

  for (uint32_t i = 0; i < index_count; i += 6)
  {
    // First triangle
    // Top left
    indices[i] = offset;
    // Top right
    indices[i + 1] = offset + 1;
    // Bottom left
    indices[i + 2] = offset + 2;

    // Second triangle
    // Bottom left
    indices[i + 3] = offset + 2;
    // Top right
    indices[i + 4] = offset + 1;
    // Bottom right
    indices[i + 5] = offset + 3;

    offset += 4;
    count += 6;
  }

  wgpuQueueWriteBuffer(queue, buffer, 0, indices, count * sizeof(uint32_t));
}

}  // namespace dl::utils
