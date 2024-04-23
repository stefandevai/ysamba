#pragma once

#include <glm/glm.hpp>
#include <memory>

#include "./scene.hpp"
#include "core/input_manager.hpp"
#include "core/maths/vector.hpp"
#include "ecs/components/sprite.hpp"
#include "graphics/quad.hpp"
#include "world/generators/biome_type.hpp"
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
  Vector2i panel_margin{15, 15};
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
  std::unique_ptr<Texture> m_biome_texture = nullptr;
  int m_seed = 0;
  std::vector<float> m_height_map{};
  std::vector<float> m_sea_distance_field{};
  std::vector<BiomeType> m_biome_map{};
  InputManager& m_input_manager = InputManager::get_instance();
  ui::WorldCreationPanel* m_panel = nullptr;

  Quad m_location_selector{0, 0, Color{0xFFFFFFFF}};
  Quad m_location_selector_on_hover{0, 0, Color{0xFF22BBAA}};
  Vector2i m_location_selector_position{0, 0};
  Vector2i m_location_selector_on_hover_position{0, 0};
  bool m_has_selected_location = false;
  bool m_is_hovering_map = false;
  static constexpr int m_texels_per_cell = 4;

  void m_generate_map();
  void m_generate_world();
  void m_create_map_representation();
  void m_create_biome_representation();
  bool m_update_input();
  void m_update_world_location_selection();
};
}  // namespace dl
