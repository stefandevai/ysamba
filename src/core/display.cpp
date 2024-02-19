#include "./display.hpp"

#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include <sstream>

#include "graphics/renderer/sdl2_webgpu.h"

#if DISABLE_VSYNC == 1
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#include <OpenGL/OpenGL.h>
#endif
#endif

#include "core/maths/vector.hpp"
#include "definitions.hpp"

#ifdef DL_BUILD_DEBUG_TOOLS
#include "debug/debug_tools.hpp"

namespace
{
auto& debug_tools = dl::DebugTools::get_instance();
}
#endif

namespace
{
WGPUAdapter request_adapter(WGPUInstance instance, WGPURequestAdapterOptions const* options)
{
  struct UserData
  {
    WGPUAdapter adapter = nullptr;
    bool request_ended = false;
  };
  UserData user_data;

  auto on_adapter_request_ended =
      [](WGPURequestAdapterStatus status, WGPUAdapter adapter, char const* message, void* user_data_) {
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

  auto on_device_request_ended =
      [](WGPURequestDeviceStatus status, WGPUDevice device, char const* message, void* user_data_) {
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
}  // namespace

namespace dl
{
SDL_Window* Display::m_window = nullptr;
int Display::m_width = 0;
int Display::m_height = 0;

Display::Display() {}

void Display::load(const int width, const int height, const std::string& title)
{
  m_width = width;
  m_height = height;
  m_title = title;

  if (SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    spdlog::critical("It was not possible to initialize SDL2");
    return;
  }

  // const SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  const SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED);
  m_window = SDL_CreateWindow(
      m_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_width, m_height, window_flags);
  SDL_SetWindowMinimumSize(m_window, width, height);

  WGPUInstanceDescriptor desc = {};
  desc.nextInChain = nullptr;
  WGPUInstance instance = wgpuCreateInstance(&desc);

  if (instance == nullptr)
  {
    spdlog::critical("Failed to create WebGPU instance");
    return;
  }

  surface = SDL_GetWGPUSurface(instance, m_window);

  WGPURequestAdapterOptions options = {};
  options.nextInChain = nullptr;
  options.compatibleSurface = surface;
  options.powerPreference = WGPUPowerPreference_HighPerformance;
  options.forceFallbackAdapter = false;

#if defined(__APPLE__)
  options.backendType = WGPUBackendType_Metal;
#elif defined(_WIN32)
  options.backendType = WGPUBackendType_D3D12;
#else
  options.backendType = WGPUBackendType_Vulkan;
#endif

  WGPUAdapter adapter = request_adapter(instance, &options);

  WGPUAdapterProperties properties = {};
  properties.nextInChain = nullptr;
  wgpuAdapterGetProperties(adapter, &properties);

  spdlog::info("Using adapter: {}", properties.name);

  WGPUDeviceDescriptor device_desc = {};
  device_desc.label = "Device 1";
  device_desc.requiredFeatureCount = 0;
  device_desc.requiredLimits = nullptr;
  device_desc.defaultQueue.label = "Default Queue";
  device_desc.defaultQueue.nextInChain = nullptr;
  device = request_device(adapter, &device_desc);

  auto on_device_error = [](WGPUErrorType type, const char* message, void*) {
    spdlog::critical("Device error: {}, {}", (uint32_t)type, message);
  };
  wgpuDeviceSetUncapturedErrorCallback(device, on_device_error, nullptr);

  WGPUTextureFormat surface_format = wgpuSurfaceGetPreferredFormat(surface, adapter);

  WGPUSurfaceConfiguration surface_configuration;
  surface_configuration.nextInChain = nullptr;
  surface_configuration.device = device;
  surface_configuration.format = surface_format;
  surface_configuration.usage = WGPUTextureUsage_RenderAttachment;
  surface_configuration.viewFormatCount = 1;
  surface_configuration.viewFormats = &surface_format;
  surface_configuration.alphaMode = WGPUCompositeAlphaMode_Auto;
  surface_configuration.width = m_width;
  surface_configuration.height = m_height;
  surface_configuration.presentMode = WGPUPresentMode_Fifo;

  wgpuSurfaceConfigure(surface, &surface_configuration);

  // Adapter and Instance are only needed in initialization
  wgpuAdapterRelease(adapter);
  wgpuInstanceRelease(instance);

  // #if __APPLE__
  //   // Always required on Mac
  //   SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
  //   SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  //   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  //   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  // #else
  //   SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
  //   SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  //   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  //   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
  // #endif
  //
  //   SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  //   m_gl_context = SDL_GL_CreateContext(m_window);
  //   SDL_GL_MakeCurrent(m_window, m_gl_context);
  //   SDL_GL_SetSwapInterval(1);
  //

#ifdef DL_BUILD_DEBUG_TOOLS
  debug_tools.init(m_window, m_gl_context);
#endif
}

void temp()
{
  if (!gladLoadGLLoader(SDL_GL_GetProcAddress))
  {
    spdlog::critical("Failed to initialize GLAD");
  }
}

Display::~Display()
{
  wgpuDeviceRelease(device);
  wgpuSurfaceRelease(surface);
  // SDL_GL_DeleteContext(m_gl_context);
  SDL_DestroyWindow(m_window);
  SDL_Quit();
}

const Vector2i Display::get_size() const { return {m_width, m_height}; }

const Vector2i Display::get_window_size() { return {m_width, m_height}; }

void Display::render()
{
#ifdef DL_BUILD_DEBUG_TOOLS
  debug_tools.render();
#endif
  // SDL_GL_SwapWindow(m_window);
}

void Display::set_title(const std::string& title) { SDL_SetWindowTitle(m_window, title.c_str()); }

void Display::set_size(const int width, const int height)
{
  m_width = width;
  m_height = height;

  auto flags = SDL_GetWindowFlags(m_window);

  // If the window is maximazed, don't resize it
  if (flags & SDL_WINDOW_MAXIMIZED)
  {
    return;
  }

  SDL_SetWindowSize(m_window, width, height);
}

void Display::reset_viewport() { glViewport(0, 0, m_width, m_height); }

void Display::update_viewport()
{
  int width, height;
  SDL_GetWindowSize(m_window, &width, &height);
  m_width = width;
  m_height = height;

  glViewport(0, 0, width, height);
}
}  // namespace dl
