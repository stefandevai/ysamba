#pragma once

#include <map>
#include <memory>

namespace dl
{
  class SDLInputWrapper
  {
    public:
      SDLInputWrapper();
      ~SDLInputWrapper();

      // Removing copy-constructor and assignment operator
      SDLInputWrapper(SDLInputWrapper const&) {};
      void operator=(SDLInputWrapper const&) {};

      static std::shared_ptr<SDLInputWrapper> get_instance();
      void update();
      bool is_key_down(int key);
      bool is_any_key_down();
      bool is_key_up(int key);
      bool should_quit();
      void quit();

    private:
      static std::shared_ptr<SDLInputWrapper> m_instance;
      const uint8_t* m_keyboard = nullptr;
      bool m_any_key_down = false;
      std::map<int, bool> m_key_down;
      std::map<int, bool> m_key_up;
      bool m_should_quit = false;
  };
}

