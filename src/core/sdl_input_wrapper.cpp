#include <SDL.h>
#include <spdlog/spdlog.h>

#include "./input_manager.hpp"

namespace dl
{
std::shared_ptr<SDLInputWrapper> SDLInputWrapper::m_instance = nullptr;

SDLInputWrapper::SDLInputWrapper() {}

std::shared_ptr<SDLInputWrapper> SDLInputWrapper::get_instance()
{
  if (m_instance == nullptr)
  {
    m_instance = std::make_shared<SDLInputWrapper>();
  }

  return m_instance;
}

void SDLInputWrapper::update()
{
  // Block until events exist
  /* SDL_WaitEvent(nullptr); */

  // Reset keys
  for (auto& it : m_key_down)
  {
    it.second = false;
  }

  for (auto& it : m_key_up)
  {
    it.second = false;
  }

  m_any_key_down = false;

  // Update key status
  SDL_Event event;

  while (SDL_PollEvent(&event))
  {
    switch (event.type)
    {
    case SDL_QUIT:
    {
      m_should_quit = true;
      break;
    }

    case SDL_KEYDOWN:
    {
      m_keyboard = SDL_GetKeyboardState(nullptr);
      int index = event.key.keysym.scancode;

      spdlog::debug("Pressed key code: {}", index);

      m_key_down[index] = true;
      m_any_key_down = true;
      break;
    }

    case SDL_KEYUP:
    {
      m_keyboard = SDL_GetKeyboardState(nullptr);
      int index = event.key.keysym.scancode;
      m_key_up[index] = true;
      break;
    }

    case SDL_MOUSEMOTION:
    {
      m_mouse_position.first = event.motion.x;
      m_mouse_position.second = event.motion.y;
      break;
    }

    case SDL_MOUSEBUTTONDOWN:
    {
      if (event.button.button == SDL_BUTTON_LEFT)
      {
        m_mouse_state.first = true;
      }
      else if (event.button.button == SDL_BUTTON_RIGHT)
      {
        m_mouse_state.second = true;
      }
      break;
    }

    case SDL_MOUSEBUTTONUP:
    {
      if (event.button.button == SDL_BUTTON_LEFT)
      {
        m_mouse_state.first = false;
      }
      else if (event.button.button == SDL_BUTTON_RIGHT)
      {
        m_mouse_state.second = false;
      }
      break;
    }

    default:
      break;
    }
  }
}

bool SDLInputWrapper::is_key_down(int key)
{
  auto it = m_key_down.find(key);

  if (it == m_key_down.end())
  {
    return false;
  }

  return it->second;
}

bool SDLInputWrapper::is_any_key_down() { return m_any_key_down; }

bool SDLInputWrapper::is_key_up(int key)
{
  auto it = m_key_up.find(key);

  if (it == m_key_up.end())
  {
    return false;
  }

  return it->second;
}

bool SDLInputWrapper::should_quit() { return m_should_quit; }

void SDLInputWrapper::quit() { m_should_quit = true; }
}  // namespace dl
