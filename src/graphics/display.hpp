#pragma once

#include <webgpu/wgpu.h>

#include "core/maths/vector.hpp"
#include "graphics/renderer/wgpu_context.hpp"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) || defined(__APPLE__)
#define SDL_MAIN_HANDLED
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif
#include <glm/glm.hpp>
#include <string>

namespace dl
{
class Display
{
 public:
  WGPUContext wgpu_context{};

  Display() = default;
  ~Display();
  void load(int width, int height, const std::string& title);
  void set_title(const std::string& title);
  void set_size(int width, int height);
  static const Vector2i& get_window_size();
  static const Vector2i& get_physical_size();
  static const Vector2& get_pixel_scale();
  void update_viewport();

 private:
  bool m_has_loaded = false;

  inline static SDL_Window* m_window = nullptr;
  inline static Vector2i m_window_size{};
  inline static Vector2i m_physical_size{};
  inline static Vector2 m_pixel_scale{};
  std::string m_title;

  void m_configure_surface();
};
}  // namespace dl
