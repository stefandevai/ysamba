#pragma once

#include <glm/glm.hpp>
#include <memory>

#include "./scene.hpp"
#include "core/input_manager.hpp"
#include "core/maths/vector.hpp"
#include "ecs/components/sprite.hpp"
#include "world/world.hpp"

namespace dl::ui
{
class WorldCreationPanel;
}

namespace dl
{
struct Quad;
struct GameContext;
class Texture;
using namespace entt::literals;

class WorldCreation : public Scene
{
 public:
  Vector3i world_size{256, 256, 30};
  WorldCreation(GameContext& game_context);

  void load() override;
  void update() override;
  void render() override;

  bool will_quit = false;

  void save();

 private:
  enum class SceneState
  {
    Normal,
    Pop,
  };
  SceneState m_scene_state = SceneState::Normal;
  World m_world{m_game_context};
  std::unique_ptr<Texture> m_texture = nullptr;
  int m_seed = 0;
  std::vector<float> m_height_map{};
  InputManager& m_input_manager = InputManager::get_instance();
  ui::WorldCreationPanel* m_panel = nullptr;

  void m_generate_map();
  void m_generate_world();
  void m_create_map_representation();
  bool m_update_input();
};
}  // namespace dl
