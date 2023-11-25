#pragma once

#include <memory>

#include "./asset_manager.hpp"
#include "./display.hpp"
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
  Renderer m_renderer{m_asset_manager};
  Camera m_camera;
  std::shared_ptr<InputManager> m_input_manager = InputManager::get_instance();
  SceneManager m_scene_manager{m_camera};

  void m_handle_window_size_change();
};
}  // namespace dl
