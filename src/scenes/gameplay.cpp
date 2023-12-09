#include "./gameplay.hpp"

#include <spdlog/spdlog.h>

#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/queue.hpp>
#include <cereal/types/stack.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/types/vector.hpp>
#include <climits>
#include <entt/core/hashed_string.hpp>
#include <entt/entity/snapshot.hpp>
#include <fstream>

#include "core/game_context.hpp"
#include "core/json.hpp"
#include "core/scene_manager.hpp"
#include "ecs/components/action_pickup.hpp"
#include "ecs/components/action_walk.hpp"
#include "ecs/components/biology.hpp"
#include "ecs/components/carried_items.hpp"
#include "ecs/components/item.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/selectable.hpp"
#include "ecs/components/society_agent.hpp"
#include "ecs/components/velocity.hpp"
#include "ecs/components/visibility.hpp"
#include "ecs/components/walk_path.hpp"
#include "graphics/camera.hpp"
#include "graphics/text.hpp"
#include "world/society/society_generator.hpp"

namespace dl
{
Gameplay::Gameplay(GameContext& game_context) : Scene("gameplay", game_context) {}

void Gameplay::load()
{
  using namespace entt::literals;

  Scene::load();

  m_renderer.add_layer("world"_hs, "default");
  m_renderer.add_layer("ui"_hs, "default", 10);
  m_renderer.add_layer("ui-2"_hs, "default", 11);

  m_renderer.get_layer("ui"_hs)->has_depth = false;
  m_renderer.get_layer("ui-2"_hs)->has_depth = false;

  load_world("./world.dl");
  /* m_world.load("./data/world/test_map.json"); */

  m_camera.set_tile_size(m_world.get_tile_size());

  m_fps_text = m_registry.create();
  m_registry.emplace<Text>(m_fps_text, "FPS: ");
  m_registry.emplace<Position>(m_fps_text, 30, 30, 20);
  auto& text_component = m_registry.get<Text>(m_fps_text);
  text_component.set_is_static(false);

  /* auto society_blueprint = m_world.get_society("otomi"_hs); */
  /* auto components = SocietyGenerator::generate_members(society_blueprint); */
  /* SocietyGenerator::place_members(components, m_world, m_camera, m_registry); */

  m_has_loaded = true;
}

double delay = 0.0;

void Gameplay::update()
{
  using namespace entt::literals;

  if (!has_loaded())
  {
    return;
  }

  const auto& current_context = m_input_manager.get_current_context();

  if (current_context != nullptr && current_context->key == "gameplay"_hs)
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
  m_inspector_system.update(m_registry, m_camera);
  m_inventory_system.update(m_registry);

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
  using namespace entt::literals;

  if (!has_loaded())
  {
    return;
  }

  m_renderer.push_matrix("world"_hs, m_camera.get_view_matrix());
  m_render_system.render(m_registry, m_renderer, m_camera);
  m_renderer.pop_matrix("world"_hs);

  m_ui_manager.render(m_renderer);
}

void Gameplay::save_world(const std::string& file_path)
{
  std::ofstream output{file_path};
  cereal::BinaryOutputArchive archive{output};

  archive(m_world);

  std::ofstream output2{"game.dl"};
  cereal::BinaryOutputArchive archive2{output2};

  /* std::stringstream storage; */
  /* cereal::JSONOutputArchive output{storage}; */

  entt::snapshot{m_registry}
      .get<entt::entity>(archive2)
      .get<Position>(archive2)
      .get<Velocity>(archive2)
      .get<Biology>(archive2)
      .get<CarriedItems>(archive2)
      .get<WalkPath>(archive2)
      .get<Visibility>(archive2)
      .get<SocietyAgent>(archive2)
      .get<Item>(archive2);
}

void Gameplay::load_world(const std::string& file_path)
{
  std::ifstream input_stream{file_path};
  cereal::BinaryInputArchive archive{input_stream};
  archive(m_world);

  std::ifstream input{"game.dl"};
  cereal::BinaryInputArchive archive2{input};

  entt::snapshot_loader{m_registry}
      .get<entt::entity>(archive2)
      .get<Position>(archive2)
      .get<Velocity>(archive2)
      .get<Biology>(archive2)
      .get<CarriedItems>(archive2)
      .get<WalkPath>(archive2)
      .get<Visibility>(archive2)
      .get<SocietyAgent>(archive2)
      .get<Item>(archive2);

  m_has_loaded = true;
}

void Gameplay::m_update_input(GameContext& m_game_context)
{
  using namespace entt::literals;

  if (m_input_manager.poll_action("quit"_hs))
  {
    m_game_context.scene_manager->pop_scene();
  }
  else if (m_input_manager.poll_action("toggle_pause"_hs))
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
  else if (m_input_manager.poll_action("save_world"_hs))
  {
    save_world("./world.dl");
  }
  else if (m_input_manager.poll_action("load_world"_hs))
  {
    load_world("./world.dl");
  }
  else if (m_input_manager.poll_action("add_item"_hs))
  {
    JSON json{"./data/debug/item.json"};
    const auto id = json.object["id"].get<uint32_t>();

    const auto& mouse_position = m_input_manager.get_mouse_position();
    const auto& camera_position = m_camera.get_position();
    const auto& tile_size = m_camera.get_tile_size();

    const int tile_x = (mouse_position.x + camera_position.x) / tile_size.x;
    const int tile_y = (mouse_position.y + camera_position.y) / tile_size.y;

    const auto item = m_registry.create();
    m_registry.emplace<Position>(item, tile_x, tile_y, 1);
    m_registry.emplace<Visibility>(item, m_world.get_texture_id(), id, "item", 1);
    m_registry.emplace<Item>(item, id);
  }
  else if (m_input_manager.poll_action("display_seed"_hs))
  {
    spdlog::info("SEED: {}", m_world.get_seed());
  }
  else if (m_input_manager.poll_action("camera_move_west"_hs))
  {
    m_camera.move({-8., 0., 0.});
  }
  else if (m_input_manager.poll_action("camera_move_east"_hs))
  {
    m_camera.move({8., 0., 0.});
  }
  else if (m_input_manager.poll_action("camera_move_south"_hs))
  {
    m_camera.move({0., 8., 0.});
  }
  else if (m_input_manager.poll_action("camera_move_north"_hs))
  {
    m_camera.move({0., -8., 0.});
  }
}
}  // namespace dl
