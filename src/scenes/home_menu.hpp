#pragma once

#include "./scene.hpp"
#include "../core/input_manager.hpp"

namespace dl
{
  class Renderer;

  class HomeMenu : public Scene
  {
    public:
      HomeMenu(const std::string &scene_key);

      void load() override;
      void update(const uint32_t delta, std::function<void(const std::string&)> set_scene) override;
      void render(Renderer& renderer) override;

    private:
      std::shared_ptr<InputManager> m_input_manager = InputManager::get_instance();
      std::string m_game_title;
      std::string m_instructions;
  };
}
