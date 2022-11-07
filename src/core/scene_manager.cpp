#include "./scene_manager.hpp"
#include "../scenes/home_menu.hpp"
#include "../scenes/gameplay.hpp"

namespace dl
{
  std::shared_ptr<Scene> SceneManager::m_current_scene = nullptr;

  std::map<std::string, SceneType> SceneManager::m_scenes_data = std::map<std::string, SceneType>{
    {"home_menu", SceneType::HOME_MENU},
    {"gameplay", SceneType::GAMEPLAY},
  };

  SceneManager::SceneManager()
  {
    m_load();
  }

  void SceneManager::set_scene(const std::string& key)
  {
    const auto it = m_scenes_data.find(key);

    if (it == m_scenes_data.end())
    {
      return;
    }

    const auto scene_type = it->second;

    switch(scene_type)
    {
      case SceneType::HOME_MENU:
        m_current_scene = std::make_shared<HomeMenu>();
        break;
      case SceneType::GAMEPLAY:
        m_current_scene = std::make_shared<Gameplay>();
        break;
      default:
        break;
    }
  }

  void SceneManager::update(const uint32_t delta)
  {
    if (m_current_scene == nullptr)
    {
      return;
    }

    if (!m_current_scene->has_loaded())
    {
      m_current_scene->load();
    }

    m_current_scene->update(delta, SceneManager::set_scene);
  }

  void SceneManager::render(TCOD_Console& console)
  {
    if (m_current_scene == nullptr)
    {
      return;
    }

    m_current_scene->render(console);
  }

  void SceneManager::m_load()
  {
    m_inital_scene_key = m_lua.get_variable<std::string>("initial_scene");
    set_scene(m_inital_scene_key);
  }
}
