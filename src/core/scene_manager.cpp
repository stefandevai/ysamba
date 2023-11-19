#include "./scene_manager.hpp"

#include <spdlog/spdlog.h>

#include <memory>

#include "../graphics/camera.hpp"
#include "../scenes/gameplay.hpp"
#include "../scenes/home_menu.hpp"
#include "../scenes/world_creation.hpp"

namespace dl
{
std::shared_ptr<Scene> SceneManager::m_current_scene = nullptr;

std::map<std::string, SceneType> SceneManager::m_scenes_data = std::map<std::string, SceneType>{
    {"home_menu", SceneType::HOME_MENU},
    {"world_creation", SceneType::WORLD_CREATION},
    {"gameplay", SceneType::GAMEPLAY},
};

SceneManager::SceneManager(ViewCamera& camera) : m_camera(camera) { m_load(); }

void SceneManager::set_scene(const std::string& key, ViewCamera& camera)
{
  const auto it = m_scenes_data.find(key);

  if (it == m_scenes_data.end())
  {
    spdlog::critical("Could not find scene: ", key);
    return;
  }

  const auto scene_type = it->second;

  switch (scene_type)
  {
  case SceneType::HOME_MENU:
    m_current_scene = std::make_shared<HomeMenu>(key, camera);
    break;
  case SceneType::WORLD_CREATION:
    m_current_scene = std::make_shared<WorldCreation>(key, camera);
    break;
  case SceneType::GAMEPLAY:
    m_current_scene = std::make_shared<Gameplay>(key, camera);
    break;
  default:
    spdlog::critical("Could not find scene: ", key);
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

void SceneManager::render(Renderer& renderer)
{
  if (m_current_scene == nullptr || !m_current_scene->has_loaded())
  {
    return;
  }

  m_current_scene->render(renderer);
}

/* void SceneManager::screenshot(tcod::Context& context, TCOD_Console& console, const std::string& filename) */
/* { */
/*   m_current_scene->screenshot(context, console, filename); */
/* } */

void SceneManager::m_load()
{
  m_inital_scene_key = m_json.object["initial_scene"];
  set_scene(m_inital_scene_key, m_camera);
}
}  // namespace dl
