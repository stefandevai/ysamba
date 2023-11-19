#pragma once

#include <filesystem>
#include <string>

#include "../core/lua_api.hpp"

namespace dl
{
class Renderer;
class Camera;
using SetSceneFunction = std::function<void(const std::string&, Camera& camera)>;

class Scene
{
 public:
  Scene(const std::string& scene_key, Camera& camera);
  virtual ~Scene();

  virtual void load();
  virtual void update(const double delta, SetSceneFunction set_scene) = 0;
  virtual void render(Renderer& renderer) = 0;
  /* virtual void screenshot(tcod::Context& context, TCOD_Console& console, const std::string& filename); */
  inline bool has_loaded() const { return m_has_loaded; };

 protected:
  const std::string m_scene_key;
  const std::filesystem::path m_scene_dir;
  Camera& m_camera;
  LuaAPI m_lua;
  bool m_has_loaded = false;
};
}  // namespace dl
