#pragma once

#include <map>
#include <memory>

#include "./json.hpp"
#include "core/input_manager.hpp"
#include "scenes/scene.hpp"

namespace dl
{
enum SceneType
{
  HOME_MENU,
  WORLD_CREATION,
  GAMEPLAY,
};

class Renderer;
struct GameContext;

class SceneManager
{
 public:
  SceneManager() {}

  template <typename Type, typename... Args>
  void push_scene(Args&&... args)
  {
    auto scene = std::make_unique<Type>(std::forward<Args>(args)...);
    InputManager::get_instance()->push_context(scene->get_key());
    m_scenes.push_back(std::move(scene));
  }

  void pop_scene();
  void update(GameContext& game_context);
  void render();

 private:
  JSON m_json{"./data/game.json"};
  std::string m_inital_scene_key;
  std::vector<std::unique_ptr<Scene>> m_scenes;

  void m_load();
};
}  // namespace dl
