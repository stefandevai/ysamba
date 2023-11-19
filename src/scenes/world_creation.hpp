#pragma once

#include <glm/glm.hpp>
#include <memory>

#include "../core/input_manager.hpp"
#include "../world/world.hpp"
#include "./scene.hpp"

namespace dl
{
class Renderer;
class Camera;
class Sprite;
struct Quad;

class WorldCreation : public Scene
{
 public:
  WorldCreation(const std::string& scene_key, Camera& camera);

  void load() override;
  void update(const double delta, SetSceneFunction set_scene) override;
  void render(Renderer& renderer) override;
  /* void screenshot(tcod::Context& context, TCOD_Console& console, const std::string& filename) override; */

  void save_world(const std::string& file_path);
  void load_world(const std::string& file_path);

 private:
  using WorldRepresentation = std::vector<std::pair<glm::vec2, std::shared_ptr<Quad>>>;
  World m_world;
  std::shared_ptr<Sprite> m_world_sprite;
  WorldRepresentation m_world_representation;
  std::shared_ptr<InputManager> m_input_manager = InputManager::get_instance();
  int m_seed = 110;
  bool m_should_update_world_representation = false;

  void m_generate_map(const int seed = 0);
  void m_create_world_representation();
};
}  // namespace dl
