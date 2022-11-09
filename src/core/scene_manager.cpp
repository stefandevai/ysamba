#include "./scene_manager.hpp"

#include <memory>
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

  void SceneManager::render(tcod::Context& context, TCOD_Console& console)
  {
    if (m_current_scene == nullptr)
    {
      return;
    }

    auto gameplay_scene = std::dynamic_pointer_cast<Gameplay>(m_current_scene);

    if (gameplay_scene != nullptr)
    {
      gameplay_scene->render_map(context);
    }
    else
    {
      console.clear();
      m_current_scene->render(console);
      context.present(console);
    }
  }

  void SceneManager::screenshot(tcod::Context& context, TCOD_Console& console, const std::string& filename)
  {
    m_current_scene->screenshot(context, console, filename);
  }

  void SceneManager::m_load()
  {
    m_inital_scene_key = m_lua.get_variable<std::string>("initial_scene");
    set_scene(m_inital_scene_key);
  }
}
