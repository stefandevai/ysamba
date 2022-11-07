#pragma once

#include <memory>
#include "./sdl_input_wrapper.hpp"
#include "./lua_api.hpp"

namespace dl
{
  class InputManager
  {
    public:
      InputManager();

      // Removing copy-constructor and assignment operator
      InputManager(InputManager const&) {};
      void operator=(InputManager const&) {};

      static std::shared_ptr<InputManager> get_instance();
      void update();
      bool is_action_down(const std::string& action);
      bool is_key_down(int key);
      bool is_any_key_down();
      bool is_key_up(int key);
      bool should_quit();
      void quit();

    private:
      static std::shared_ptr<InputManager> m_instance;
      static std::shared_ptr<SDLInputWrapper> m_sdl_input_wrapper;
      LuaAPI m_lua{"key_bindings.lua"};
  };
}
