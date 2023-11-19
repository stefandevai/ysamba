#pragma once

#include <map>
#include <memory>

#include "../scenes/scene.hpp"
#include "./json.hpp"

namespace dl
{
enum SceneType
{
  HOME_MENU,
  WORLD_CREATION,
  GAMEPLAY,
};

class Renderer;
class Camera;

class SceneManager
{
 public:
  SceneManager(Camera& camera);

  static void set_scene(const std::string& key, Camera& camera);
  void update(const uint32_t delta);
  void render(Renderer& renderer);
  /* void screenshot(tcod::Context& context, TCOD_Console& console, const std::string& filename); */

  inline std::shared_ptr<Scene> get_current_scene() const { return m_current_scene; };

 private:
  Camera& m_camera;
  JSON m_json{"./data/game.json"};
  std::string m_inital_scene_key;
  static std::shared_ptr<Scene> m_current_scene;
  static std::map<std::string, SceneType> m_scenes_data;

  void m_load();
};
}  // namespace dl
