#pragma once

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) || defined(__APPLE__)
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif
#include <glm/glm.hpp>
#include <string>

#include "core/maths/vector.hpp"

namespace dl
{
class Display
{
 public:
  Display();
  ~Display();
  void load(const int width, const int height, const std::string& title);
  void render();
  void clear();
  void set_title(const std::string& title);
  void set_size(const int width, const int height);
  void set_clear_color(const float a, const float b, const float c);
  const Vector2i get_size() const;
  const Vector2i get_window_size() const;
  /* unsigned int get_width() const; */
  /* unsigned int get_height() const; */
  /* int get_window_width() const; */
  /* int get_window_height() const; */
  void reset_viewport();
  void update_viewport();

 private:
  SDL_Window* m_window;
  SDL_GLContext m_gl_context;
  int m_width;
  int m_height;
  std::string m_title;
  glm::vec3 m_clear_color{0.0f, 0.0f, 0.0f};
};
}  // namespace dl
