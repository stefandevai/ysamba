#pragma once

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) || defined(__APPLE__)
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
  Display();
  ~Display();
  void load(const unsigned int width, const unsigned int height, const std::string& title);
  void render();
  void clear();
  void set_title(const std::string& title);
  void set_size(const int width, const int height);
  void set_clear_color(const float a, const float b, const float c);
  unsigned int get_width() const;
  unsigned int get_height() const;
  int get_window_width() const;
  int get_window_height() const;
  void reset_viewport();
  void update_viewport();

 private:
  SDL_Window* m_window;
  SDL_GLContext m_gl_context;
  unsigned int m_width;
  unsigned int m_height;
  std::string m_title;
  glm::vec3 m_clear_color{0.0f, 0.0f, 0.0f};
};
}  // namespace dl
