#include "./input_manager.hpp"

#include <vector>
#include <SDL.h>
#include <spdlog/spdlog.h>

namespace dl
{
  std::shared_ptr<InputManager> InputManager::m_instance = nullptr;
  std::shared_ptr<SDLInputWrapper> InputManager::m_sdl_input_wrapper = SDLInputWrapper::get_instance();
  std::shared_ptr<sol::table> InputManager::m_context = nullptr;
  std::string InputManager::m_context_key = "";

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
    m_sdl_input_wrapper->update();
  }

  void InputManager::set_context(const std::string& context_key)
  {
    const auto context = m_lua.get_variable<sol::table>(context_key);

    if (context == sol::lua_nil)
    {
      spdlog::critical("Could not load context: {}", context_key);
      return;
    }

    m_context = std::make_shared<sol::table>(context);
    m_context_key = context_key;
  }

  bool InputManager::poll_action(const std::string& action)
  {
    if (m_context == nullptr)
    {
      spdlog::critical("Current context not loaded: {}", m_context_key);
      return false;
    }

    const auto action_keys = m_context->get_or<std::vector<int>>(action, {});

    if (action_keys.size() == 0)
    {
      spdlog::critical("Could not find action in this context: {}", action);
      return false;
    }

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
    if (key < 0)
    {
      return is_any_key_down();
    }

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

  bool InputManager::is_clicking(const MouseButton button)
  {
    const auto& mouse_state = m_sdl_input_wrapper->get_mouse_state();

    if (button == MouseButton::Left)
    {
      return mouse_state.first;
    }
    else if (button == MouseButton::Right)
    {
      return mouse_state.second;
    }

    return false;
  }

  std::pair<int, int> InputManager::get_mouse_position()
  {
    return m_sdl_input_wrapper->get_mouse_position();
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
