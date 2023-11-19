#pragma once

#include "../core/input_manager.hpp"
#include "../graphics/text.hpp"
#include "./scene.hpp"

namespace dl
{
class Renderer;
class Camera;

class HomeMenu : public Scene
{
 public:
  HomeMenu(const std::string& scene_key, Camera& camera);

  void load() override;
  void update(const uint32_t delta, SetSceneFunction set_scene) override;
  void render(Renderer& renderer) override;

 private:
  std::shared_ptr<InputManager> m_input_manager = InputManager::get_instance();
  Text m_game_title;
  Text m_instructions;
};
}  // namespace dl
