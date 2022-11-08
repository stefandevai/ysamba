#include <vector>
#include <SDL.h>
#include "./input_manager.hpp"

namespace dl
{
  std::shared_ptr<InputManager> InputManager::m_instance = nullptr;
  std::shared_ptr<SDLInputWrapper> InputManager::m_sdl_input_wrapper = SDLInputWrapper::get_instance();

  InputManager::InputManager() { }

  std::shared_ptr<InputManager> InputManager::get_instance()
  {
    if (m_instance == nullptr)
    {
      m_instance = std::make_shared<InputManager>();
    }

    return m_instance;
  }

  void InputManager::update()
  {
    if(is_action_down("quit"))
    {
      quit();
    }

    m_sdl_input_wrapper->update();
  }

  bool InputManager::is_action_down(const std::string& action)
  {
    const auto action_keys = m_lua.get_variable<std::vector<int>>(action);

    for (const auto key : action_keys)
    {
      if (is_key_down(key))
      {
        return true;
      }
    }

    return false;
  }

  bool InputManager::is_key_down(int key)
  {
    return m_sdl_input_wrapper->is_key_down(key);
  }

  bool InputManager::is_any_key_down()
  {
    return m_sdl_input_wrapper->is_any_key_down();
  }

  bool InputManager::is_key_up(int key)
  {
    return m_sdl_input_wrapper->is_key_up(key);
  }

  bool InputManager::should_quit()
  {
    return m_sdl_input_wrapper->should_quit();
  }

  void InputManager::quit()
  {
    m_sdl_input_wrapper->quit();
  }
}
