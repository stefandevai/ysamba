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
#include "core/json.hpp"
#include "ecs/components/biology.hpp"
#include "ecs/components/item.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/selectable.hpp"
#include "ecs/components/society_agent.hpp"
#include "ecs/components/velocity.hpp"
#include "ecs/components/visibility.hpp"
#include "graphics/text.hpp"
#include "ui/components/action_menu.hpp"
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

  m_renderer.add_layer("world", "default");
  m_renderer.add_layer("ui", "default", 10);
  m_renderer.add_layer("ui-2", "default", 11);

  m_renderer.get_layer("ui")->has_depth = false;
  m_renderer.get_layer("ui-2")->has_depth = false;

  /* load_world("./world.dl"); */
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
  /* auto container = std::make_shared<ui::Container>(Vector2i{200, 300}, "#1b2420aa"); */
  /* container->position.x = 200; */
  /* container->position.y = 200; */

  /* container->children.push_back(button); */
  /* m_ui_manager.add_component(container); */

  /* const std::vector<std::string> items = { */
  /*     "Item 1", */
  /*     "Item 2", */
  /*     "Item 2", */
  /*     "Item 2", */
  /*     "Item 2", */
  /*     "Item 2", */
  /*     "Item 2", */
  /*     "Item 2", */
  /*     "Item 2", */
  /*     "Item 2", */
  /*     "Item 2", */
  /*     "Item 2", */
  /*     "Item 2", */
  /* }; */

  /* const auto on_select = [](const int i) { spdlog::debug("SELECTDFFJDK {}", i); }; */

  /* const auto action_menu = std::make_shared<ui::ActionMenu>(items, on_select); */
  /* action_menu->position.x = 100; */
  /* action_menu->position.y = 100; */
  /* m_ui_manager.add_component(action_menu); */

  /* auto button = std::make_shared<ui::Button>("CLICK HERE", Vector2i{ 100, 50 }); */
  /* button->on_click = []() { spdlog::warn("CLICKED HERE!"); }; */
  /* m_ui_manager.add_component(button); */
  /* ui::ListStyle list_style{{15, 0}, 25}; */
  /* const auto list = std::make_shared<ui::List>(items, list_style); */

  /* const auto scrollable = std::make_shared<ui::Scrollable>(); */
  /* /1* scrollable->position.x = 100; *1/ */
  /* /1* scrollable->position.y = 100; *1/ */
  /* scrollable->size.x = 200; */
  /* scrollable->size.y = 300; */

  /* scrollable->children.push_back(list); */
  /* container->children.push_back(scrollable); */

  /* m_ui_manager.add_component(container); */
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

  const auto delta = m_game_context.clock->delta;

  if (m_current_state == State::PLAYING)
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
      m_walk_system.update(m_registry);
      m_society_system.update(m_registry, delta);
      m_pickup_system.update(m_registry);
      m_job_system.update(m_registry, delta);
    }
  }

  m_action_system.update(m_registry, m_camera);
  m_inventory_system.update(m_registry, m_camera);
  m_inspector_system.update(m_registry, m_camera);

  m_ui_manager.update();

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

  m_renderer.push_matrix("world", m_camera.get_view_matrix());
  m_render_system.render(m_registry, m_renderer, m_camera);
  m_renderer.pop_matrix("world");

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
    if (m_current_state == State::PLAYING)
    {
      m_current_state = State::PAUSED;
    }
    else
    {
      m_current_state = State::PLAYING;
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
  else if (m_input_manager->poll_action("add_item"))
  {
    JSON json{"./data/debug/item.json"};
    const auto id = json.object["id"].get<uint32_t>();

    const auto& mouse_position = m_input_manager->get_mouse_position();
    const auto& camera_position = m_camera.get_position();
    const auto& tile_size = m_camera.get_tile_size();

    const int tile_x = (mouse_position.x + camera_position.x) / tile_size.x;
    const int tile_y = (mouse_position.y + camera_position.y) / tile_size.y;

    const auto item = m_registry.create();
    m_registry.emplace<Position>(item, tile_x, tile_y, 1);
    m_registry.emplace<Visibility>(item, m_world.get_texture_id(), id, "item", 1);
    m_registry.emplace<Item>(item, id);
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
