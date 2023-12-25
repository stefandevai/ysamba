#pragma once

#include <map>
#include <memory>

#include "./json.hpp"
#include "core/display.hpp"
#include "core/input_manager.hpp"
#include "scenes/scene.hpp"

namespace dl
{
class Renderer;
class Display;

class SceneManager
{
 public:
  SceneManager(Display& display) : m_display(display) {}

  template <typename Type, typename... Args>
  void push_scene(Args&&... args)
  {
    auto scene = std::make_unique<Type>(std::forward<Args>(args)...);
    InputManager::get_instance().push_context(scene->get_key());
    m_scenes.push_back(std::move(scene));
  }

  void pop_scene();
  void update();
  void render();

 private:
  Display& m_display;
  JSON m_json{"./data/game.json"};
  std::vector<std::unique_ptr<Scene>> m_scenes;

  void m_load();
  void m_check_window_size();
};
}  // namespace dl
