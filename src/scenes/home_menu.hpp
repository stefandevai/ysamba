#pragma once

#include <libtcod.hpp>
#include "./scene.hpp"
#include "../core/input_manager.hpp"

namespace dl
{
  class HomeMenu : public Scene
  {
    public:
      HomeMenu();
      ~HomeMenu();

      void load() override;
      void update(const uint32_t delta, std::function<void(const std::string&)> set_scene) override;
      void render(TCOD_Console& console) override;

    private:
      std::shared_ptr<InputManager> m_input_manager = InputManager::get_instance();
      std::string m_game_title;
      std::string m_instructions;
  };
}
