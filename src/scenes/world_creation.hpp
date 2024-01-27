#pragma once

#include <glm/glm.hpp>
#include <memory>

#include "./scene.hpp"
#include "core/input_manager.hpp"
#include "world/world.hpp"

namespace dl
{
class Sprite;
struct Quad;
struct GameContext;

class WorldCreation : public Scene
{
 public:
  WorldCreation(GameContext& game_context);

  void load() override;
  void update() override;
  void render() override;
  /* void screenshot(tcod::Context& context, TCOD_Console& console, const std::string& filename) override; */

  void save_world();
  void load_world();

 private:
  World m_world{m_game_context};
  std::unique_ptr<Sprite> m_world_sprite = nullptr;
  std::vector<float> m_height_map{};
  InputManager& m_input_manager = InputManager::get_instance();
  int m_seed = 110;

  void m_generate_map(const int seed = 0);
  void m_create_world_representation();
  void m_generate_height_map(const int seed = 0);
  void m_create_height_map_representation();
};
}  // namespace dl
