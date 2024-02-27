#pragma once

#include <webgpu/wgpu.h>

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
struct Vector2i;

class Display
{
 public:
  WGPUContext wgpu_context{};

  Display() = default;
  ~Display();
  void load(const int width, const int height, const std::string& title);
  void set_title(const std::string& title);
  void set_size(const int width, const int height);
  const Vector2i get_size() const;
  static const Vector2i get_window_size();
  void update_viewport();

 private:
  bool m_has_loaded = false;

  static SDL_Window* m_window;
  static int m_width;
  static int m_height;
  std::string m_title;

  void m_configure_surface();
};
}  // namespace dl
