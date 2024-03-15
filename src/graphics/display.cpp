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
  m_title = title;

  SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0");

  if (SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    spdlog::critical("It was not possible to initialize SDL2");
    return;
  }

  const SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
  // const SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_ALLOW_HIGHDPI);
  m_window = SDL_CreateWindow(
      m_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, window_flags);
  SDL_SetWindowMinimumSize(m_window, width, height);
  SDL_GetWindowSize(m_window, &m_window_size.x, &m_window_size.y);
  SDL_GL_GetDrawableSize(m_window, &m_physical_size.x, &m_physical_size.y);
  m_pixel_scale.x = m_physical_size.x / static_cast<double>(m_window_size.x);
  m_pixel_scale.y = m_physical_size.y / static_cast<double>(m_window_size.y);

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

const Vector2i& Display::get_physical_size() { return m_physical_size; }

const Vector2i& Display::get_window_size() { return m_window_size; }

const Vector2& Display::get_pixel_scale() { return m_pixel_scale; }

void Display::set_title(const std::string& title) { SDL_SetWindowTitle(m_window, title.c_str()); }

void Display::set_size(const int width, const int height)
{
  auto flags = SDL_GetWindowFlags(m_window);

  // If the window is maximazed, don't resize it
  if (flags & SDL_WINDOW_MAXIMIZED)
  {
    return;
  }

  SDL_SetWindowSize(m_window, width, height);
  SDL_GetWindowSize(m_window, &m_window_size.x, &m_window_size.y);
  SDL_GL_GetDrawableSize(m_window, &m_physical_size.x, &m_physical_size.y);
  m_pixel_scale.x = m_physical_size.x / static_cast<double>(m_window_size.x);
  m_pixel_scale.y = m_physical_size.y / static_cast<double>(m_window_size.y);
}

void Display::update_viewport()
{
  SDL_GetWindowSize(m_window, &m_window_size.x, &m_window_size.y);
  SDL_GL_GetDrawableSize(m_window, &m_physical_size.x, &m_physical_size.y);
  m_pixel_scale.x = m_physical_size.x / static_cast<double>(m_window_size.x);
  m_pixel_scale.y = m_physical_size.y / static_cast<double>(m_window_size.y);

  wgpuSurfaceRelease(wgpu_context.surface);
  wgpu_context.surface = SDL_GetWGPUSurface(wgpu_context.instance, m_window);
  m_configure_surface();
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
  surface_configuration.width = m_physical_size.x;
  surface_configuration.height = m_physical_size.y;
  surface_configuration.presentMode = WGPUPresentMode_Fifo;

  wgpuSurfaceConfigure(wgpu_context.surface, &surface_configuration);
}
}  // namespace dl
