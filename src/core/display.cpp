#include "./display.hpp"

#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include <sstream>

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

  /* const SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL); */
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

#ifdef DL_BUILD_DEBUG_TOOLS
  debug_tools.init(m_window, m_gl_context);
#endif
}

Display::~Display()
{
  SDL_GL_DeleteContext(m_gl_context);
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
  SDL_GL_SwapWindow(m_window);
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
