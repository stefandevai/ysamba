#pragma once

#include <memory>

#include "./asset_manager.hpp"
#include "./clock.hpp"
#include "./display.hpp"
#include "./game_context.hpp"
#include "./input_manager.hpp"
#include "./json.hpp"
#include "./scene_manager.hpp"
#include "graphics/camera.hpp"
#include "graphics/renderer.hpp"

namespace dl
{
class Game
{
 public:
  Game();

  void load();
  void run();

 private:
  JSON m_json{"./data/game.json"};
  Display m_display;
  AssetManager m_asset_manager{"./data/assets.json"};
  Camera m_camera;
  SceneManager m_scene_manager{};
  std::shared_ptr<InputManager> m_input_manager = InputManager::get_instance();
  Clock m_clock{};

  GameContext m_context{&m_display, &m_asset_manager, &m_camera, &m_scene_manager, &m_clock, m_input_manager};
};
}  // namespace dl
