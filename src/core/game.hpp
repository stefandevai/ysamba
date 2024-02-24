#pragma once

#include <memory>

#include "./asset_manager.hpp"
#include "./clock.hpp"
#include "./game_context.hpp"
#include "./input_manager.hpp"
#include "./json.hpp"
#include "./scene_manager.hpp"
#include "graphics/camera.hpp"
#include "graphics/display.hpp"

namespace dl
{
class Game
{
 public:
  void load();
  void run();

 private:
  Display m_display{};
  AssetManager m_asset_manager{m_display};
  SceneManager m_scene_manager{m_display};
  Camera m_camera{};
  Clock m_clock{};
  InputManager& m_input_manager = InputManager::get_instance();

  GameContext m_context{&m_display, &m_asset_manager, &m_camera, &m_scene_manager, &m_clock, {}, nullptr};
};
}  // namespace dl
