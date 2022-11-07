#include <SDL.h>
#include "./input_manager.hpp"

namespace dl
{
  std::shared_ptr<SDLInputWrapper> SDLInputWrapper::m_instance = nullptr;

  SDLInputWrapper::SDLInputWrapper() { }

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

    while(SDL_PollEvent(&event))
    {
      switch(event.type)
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

  bool SDLInputWrapper::is_any_key_down()
  {
    return m_any_key_down;
  }

  bool SDLInputWrapper::is_key_up(int key)
  {
    auto it = m_key_up.find(key);

    if (it == m_key_up.end())
    {
      return false;
    }

    return it->second;
  }

  bool SDLInputWrapper::should_quit()
  {
    return m_should_quit;
  }

  void SDLInputWrapper::quit()
  {
    m_should_quit = true;
  }
}

