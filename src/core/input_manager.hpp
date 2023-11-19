#pragma once

#include <memory>

#include "./lua_api.hpp"
#include "./sdl_input_wrapper.hpp"

namespace dl
{
class InputManager
{
 public:
  enum class MouseButton
  {
    Left,
    Right,
  };

  InputManager();

  // Removing copy-constructor and assignment operator
  InputManager(InputManager const&){};
  void operator=(InputManager const&){};

  static std::shared_ptr<InputManager> get_instance();
  void update();
  void set_context(const std::string& context_key);
  bool poll_action(const std::string& action);
  bool is_key_down(int key);
  bool is_any_key_down();
  bool is_key_up(int key);
  bool is_clicking(const MouseButton button);
  std::pair<int, int> get_mouse_position();
  bool should_quit();
  void quit();

 private:
  static std::shared_ptr<InputManager> m_instance;
  static std::shared_ptr<sol::table> m_context;
  static std::shared_ptr<SDLInputWrapper> m_sdl_input_wrapper;
  static std::string m_context_key;
  LuaAPI m_lua{"key_bindings.lua"};
};
}  // namespace dl
