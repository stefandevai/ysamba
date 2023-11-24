#include "./display.hpp"

#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include <sstream>
#include <stdexcept>

#if DISABLE_VSYNC == 1
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#include <OpenGL/OpenGL.h>
#endif
#endif

namespace dl
{
Display::Display() {}

void Display::load(const unsigned int width, const unsigned int height, const std::string& title)
{
  m_width = width;
  m_height = height;
  m_title = title;

  if (SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    throw std::runtime_error("It was not possible to initialize SDL2");
  }

  /* SDL_SetHint(SDL_HINT_MOUSE_TOUCH_EVENTS, "1"); */

  const SDL_WindowFlags window_flags =
      (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_OPENGL);
  m_window = SDL_CreateWindow(
      m_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_width, m_height, window_flags);
  SDL_SetWindowMinimumSize(m_window, width, height);

#if __APPLE__
  // Always required on Mac
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#else
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  m_gl_context = SDL_GL_CreateContext(m_window);
  SDL_GL_MakeCurrent(m_window, m_gl_context);
  SDL_GL_SetSwapInterval(1);

  if (!gladLoadGLLoader(SDL_GL_GetProcAddress))
  {
    spdlog::critical("Failed to initialize GLAD");
  }

  // OpenGL Viewport settings
  int maximized_width, maximized_height;
  SDL_GetWindowSize(m_window, &maximized_width, &maximized_height);
  glViewport(0, 0, maximized_width, maximized_height);
  m_width = maximized_width;
  m_height = maximized_height;

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);
}

Display::~Display()
{
  SDL_GL_DeleteContext(m_gl_context);
  SDL_DestroyWindow(m_window);
  SDL_Quit();
}

unsigned int Display::get_width() const { return m_width; }

unsigned int Display::get_height() const { return m_height; }

int Display::get_window_width() const
{
  int width, height;
  SDL_GetWindowSize(m_window, &width, &height);
  return width;
}

int Display::get_window_height() const
{
  int width, height;
  SDL_GetWindowSize(m_window, &width, &height);
  return height;
}

void Display::render() { SDL_GL_SwapWindow(m_window); }

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

void Display::set_clear_color(const float a, const float b, const float c)
{
  m_clear_color.x = a;
  m_clear_color.y = b;
  m_clear_color.z = c;
}

void Display::clear()
{
  glClearColor(m_clear_color.x, m_clear_color.y, m_clear_color.z, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
