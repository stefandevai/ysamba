#include "./gameplay.hpp"

#include <spdlog/spdlog.h>

#include <cereal/archives/binary.hpp>
#include <climits>
#include <fstream>

#include "core/game_context.hpp"
#include "core/scene_manager.hpp"
#include "graphics/camera.hpp"
#include "world/society/society_generator.hpp"

// TEMP
#include "ecs/components/biology.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/selectable.hpp"
#include "ecs/components/society_agent.hpp"
#include "ecs/components/velocity.hpp"
#include "ecs/components/visibility.hpp"
#include "graphics/text.hpp"
#include "ui/components/button.hpp"
#include "ui/components/container.hpp"
#include "ui/components/label.hpp"
#include "world/society/sex.hpp"
// TEMP

namespace dl
{
Gameplay::Gameplay(GameContext& game_context) : Scene("gameplay", game_context) {}

void Gameplay::load()
{
  Scene::load();

  m_renderer.add_layer("world", "world");
  m_renderer.add_layer("quad", "quad", Renderer::LayerType::Quad, true, 1);
  m_renderer.add_layer("text", "text", Renderer::LayerType::Sprite, true, 2);
  m_renderer.add_layer("ui-quad", "quad", Renderer::LayerType::Quad, true, 10);
  m_renderer.add_layer("ui-text", "text", Renderer::LayerType::Sprite, true, 10);

  load_world("./world.dl");
  m_world.load("./data/world/test_map.json");

  m_camera.set_tile_size(m_world.get_tile_size());

  m_fps_text = m_registry.create();
  m_registry.emplace<Text>(m_fps_text, "FPS: ");
  m_registry.emplace<Position>(m_fps_text, 30, 30, 20);
  auto& text_component = m_registry.get<Text>(m_fps_text);
  text_component.set_is_static(false);

  auto society_blueprint = m_world.get_society("otomi");
  auto components = SocietyGenerator::generate_members(society_blueprint);
  SocietyGenerator::place_members(components, m_world, m_camera, m_registry);

  // TEMP
  auto label = std::make_shared<ui::Label>();
  label->text.value = "TESTING UI";
  label->text.typeface = "font-1980";
  label->position.x = 80;
  label->position.y = 80;
  m_ui_manager.add_component(label);

  auto container = std::make_shared<ui::Container>(100, 100, "#1b2420aa");
  container->position.x = 100;
  container->position.y = 100;
  m_ui_manager.add_component(container);
  // TEMP

  m_has_loaded = true;
}

double delay = 0.0;

void Gameplay::update()
{
  if (!has_loaded())
  {
    return;
  }

  const auto& current_context = m_input_manager->get_current_context();

  if (current_context != nullptr && current_context->key == "gameplay")
  {
    m_update_input(m_game_context);
  }

  m_ui_manager.update();

  const auto delta = m_game_context.clock->delta;

  if (m_current_state == Gameplay::State::PLAYING)
  {
    if (m_turn_delay > 0.0)
    {
      m_turn_delay -= delta;
    }
    else
    {
      m_turn_delay = 0.5;
      m_game_system.update();
      m_physics_system.update(m_registry, delta);
      m_society_system.update(m_registry, delta);
      m_harvest_system.update(m_registry, delta);
      m_break_system.update(m_registry, delta);
    }
  }

  m_inspector_system.update(m_registry, m_camera);
  m_action_system.update(m_registry, m_camera);

  if (delay <= 0.0)
  {
    auto& text = m_registry.get<Text>(m_fps_text);
    text.set_text("FPS: " + std::to_string(1.0 / delta));
    delay = 0.8;
  }
  else
  {
    delay -= delta;
  }
}

void Gameplay::render()
{
  if (!has_loaded())
  {
    return;
  }

  m_render_system.render(m_registry, m_renderer, m_camera);
  m_ui_manager.render(m_renderer);
}

void Gameplay::save_world(const std::string& file_path)
{
  std::ofstream output_stream(file_path);
  cereal::BinaryOutputArchive archive(output_stream);
  archive(m_world);
}

void Gameplay::load_world(const std::string& file_path)
{
  std::ifstream input_stream(file_path);
  cereal::BinaryInputArchive archive(input_stream);
  archive(m_world);
  m_has_loaded = true;
}

void Gameplay::m_update_input(GameContext& m_game_context)
{
  if (m_input_manager->poll_action("quit"))
  {
    m_game_context.scene_manager->pop_scene();
  }
  else if (m_input_manager->poll_action("toggle_pause"))
  {
    if (m_current_state == Gameplay::State::PLAYING)
    {
      m_current_state = Gameplay::State::PAUSED;
    }
    else
    {
      m_current_state = Gameplay::State::PLAYING;
    }
  }
  else if (m_input_manager->poll_action("save_world"))
  {
    save_world("./world.dl");
  }
  else if (m_input_manager->poll_action("load_world"))
  {
    load_world("./world.dl");
  }
  else if (m_input_manager->poll_action("display_seed"))
  {
    spdlog::info("SEED: {}", m_world.get_seed());
  }
  else if (m_input_manager->poll_action("camera_move_west"))
  {
    m_camera.move({-8., 0., 0.});
  }
  else if (m_input_manager->poll_action("camera_move_east"))
  {
    m_camera.move({8., 0., 0.});
  }
  else if (m_input_manager->poll_action("camera_move_south"))
  {
    m_camera.move({0., 8., 0.});
  }
  else if (m_input_manager->poll_action("camera_move_north"))
  {
    m_camera.move({0., -8., 0.});
  }
}
}  // namespace dl
