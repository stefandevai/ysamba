#include "./scene.hpp"

namespace dl
{
  Scene::Scene(const std::string& scene_path)
    : m_scene_path(scene_path)
  {

  }

  Scene::~Scene()
  {

  }

  void Scene::load()
  {
    m_lua.load(m_scene_path);
  }
}

