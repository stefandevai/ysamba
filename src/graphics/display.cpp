#include "./display.hpp"

#include <spdlog/spdlog.h>

#include <array>
#include <sstream>

#include "core/maths/vector.hpp"
#include "definitions.hpp"
#include "graphics/renderer/sdl2_webgpu.h"
#include "graphics/renderer/utils.hpp"

#ifdef DL_BUILD_DEBUG_TOOLS
#include "debug/debug_tools.hpp"
#endif

namespace dl
{
SDL_Window* Display::m_window = nullptr;
int Display::m_width = 0;
int Display::m_height = 0;

Display::~Display()
{
  if (!m_has_loaded)
  {
    return;
  }

  wgpuAdapterRelease(wgpu_context.adapter);
  wgpuDeviceRelease(wgpu_context.device);
  wgpuSurfaceRelease(wgpu_context.surface);
  wgpuInstanceRelease(wgpu_context.instance);
  SDL_DestroyWindow(m_window);
  SDL_Quit();
}

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

  const SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  // const SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED);
  m_window = SDL_CreateWindow(
      m_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_width, m_height, window_flags);
  SDL_SetWindowMinimumSize(m_window, width, height);

  WGPUInstanceDescriptor desc = {
      .nextInChain = nullptr,
  };
  wgpu_context.instance = wgpuCreateInstance(&desc);
  assert(wgpu_context.instance != nullptr);

  wgpu_context.surface_format = WGPUTextureFormat_BGRA8Unorm;
  wgpu_context.surface = SDL_GetWGPUSurface(wgpu_context.instance, m_window);
  assert(wgpu_context.surface != nullptr);

  WGPURequestAdapterOptions options
      = {.compatibleSurface = wgpu_context.surface,
         .powerPreference = WGPUPowerPreference_HighPerformance,
         .forceFallbackAdapter = false,
#if defined(__APPLE__)
         .backendType = WGPUBackendType_Metal,
#elif defined(_WIN32)
         .backendType = WGPUBackendType_D3D12,
#else
         .backendType = WGPUBackendType_Vulkan,
#endif
        };

  wgpu_context.adapter = utils::request_adapter(wgpu_context.instance, &options);

  const auto properties = utils::adapter_properties(wgpu_context.adapter);
  spdlog::info("Using adapter for {}", properties.name);

  // utils::list_features(wgpu_context.adapter);

  std::array<WGPUFeatureName, 2> required_features = {
      static_cast<WGPUFeatureName>(WGPUNativeFeature_TextureBindingArray),
      static_cast<WGPUFeatureName>(WGPUNativeFeature_SampledTextureAndStorageBufferArrayNonUniformIndexing),
  };

  WGPUDeviceDescriptor device_descriptor = {
    .label = "Default Device",
    .requiredLimits = nullptr,
    .defaultQueue = {
      .label = "Default Queue",
    },
    .requiredFeatures = required_features.data(),
    .requiredFeatureCount = required_features.size(),
  };

  wgpu_context.device = utils::request_device(wgpu_context.adapter, &device_descriptor);

  // utils::device_set_uncaptured_error_callback(wgpu_context.device);

  wgpu_context.queue = wgpuDeviceGetQueue(wgpu_context.device);

  // utils::queue_on_submitted_work_done(wgpu_context.queue);

  m_configure_surface();

#ifdef DL_BUILD_DEBUG_TOOLS
  DebugTools::get_instance().init(m_window, wgpu_context.device);
#endif

  m_has_loaded = true;
}

const Vector2i Display::get_size() const { return {m_width, m_height}; }

const Vector2i Display::get_window_size() { return {m_width, m_height}; }

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

void Display::reset_viewport()
{
  // glViewport(0, 0, m_width, m_height);
}

void Display::update_viewport()
{
  int width, height;
  SDL_GetWindowSize(m_window, &width, &height);
  m_width = width;
  m_height = height;

  wgpuSurfaceRelease(wgpu_context.surface);
  wgpu_context.surface = SDL_GetWGPUSurface(wgpu_context.instance, m_window);
  m_configure_surface();
  // glViewport(0, 0, width, height);
}

void Display::m_configure_surface()
{
  WGPUSurfaceConfiguration surface_configuration;
  surface_configuration.nextInChain = nullptr;
  surface_configuration.device = wgpu_context.device;
  surface_configuration.format = wgpu_context.surface_format;
  surface_configuration.usage = WGPUTextureUsage_RenderAttachment;
  surface_configuration.viewFormatCount = 1;
  surface_configuration.viewFormats = &wgpu_context.surface_format;
  surface_configuration.alphaMode = WGPUCompositeAlphaMode_Auto;
  surface_configuration.width = m_width;
  surface_configuration.height = m_height;
  surface_configuration.presentMode = WGPUPresentMode_Fifo;

  wgpuSurfaceConfigure(wgpu_context.surface, &surface_configuration);
}
}  // namespace dl