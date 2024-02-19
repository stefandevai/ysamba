#pragma once

#include <webgpu/webgpu.h>
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
  WGPUSurface surface;

  Display();
  ~Display();
  void load(const int width, const int height, const std::string& title);
  void render();
  void set_title(const std::string& title);
  void set_size(const int width, const int height);
  const Vector2i get_size() const;
  static const Vector2i get_window_size();
  void reset_viewport();
  void update_viewport();

 private:
  static SDL_Window* m_window;
  static int m_width;
  static int m_height;
  std::string m_title;
  SDL_GLContext m_gl_context;
};
}  // namespace dl
