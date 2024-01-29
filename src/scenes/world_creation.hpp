#pragma once

#include <glm/glm.hpp>
#include <memory>

#include "./scene.hpp"
#include "core/input_manager.hpp"
#include "core/maths/vector.hpp"
#include "graphics/sprite.hpp"

namespace dl
{
struct Quad;
struct GameContext;

class WorldCreation : public Scene
{
 public:
  Vector3i world_size{256, 256, 30};
  WorldCreation(GameContext& game_context);

  void load() override;
  void update() override;
  void render() override;

  void save_world();

 private:
  Batch m_batch{"default"};
  Sprite m_world_sprite{};
  int m_seed = 0;
  std::vector<float> m_height_map{};
  InputManager& m_input_manager = InputManager::get_instance();

  void m_generate_map();
  void m_create_map_representation();
};
}  // namespace dl
