#include "./home_menu.hpp"

#include <spdlog/spdlog.h>

#include <entt/core/hashed_string.hpp>

#include "config.hpp"
#include "core/game_context.hpp"
#include "core/scene_manager.hpp"
#include "core/serialization.hpp"
#include "scenes/gameplay.hpp"
#include "scenes/world_creation.hpp"
#include "ui/compositions/world_list.hpp"
#include "world/society/name_generator.hpp"

namespace dl
{
HomeMenu::HomeMenu(GameContext& game_context) : Scene("home_menu", game_context) {}

void HomeMenu::load()
{
  using namespace entt::literals;

  Scene::load();

  m_renderer.add_batch("text"_hs, "default", 2);

  const auto game_title = m_json.object["game_title"].get<std::string>();
  const auto instructions = m_json.object["instructions"].get<std::string>();

  m_game_title.set_font_size(16);
  m_game_title.set_typeface("font-1980");
  m_game_title.set_text(game_title);

  m_instructions.set_font_size(16);
  m_instructions.set_typeface("font-1980");
  m_instructions.set_text(instructions);

  m_world_list = m_ui_manager.emplace<ui::WorldList>();
  const auto on_select_world = [this](const WorldMetadata& world_metadata) {
    m_world_list->hide();
    m_game_context.world_metadata = world_metadata;
    m_game_context.scene_manager->push_scene<Gameplay>(m_game_context);
  };
  m_world_list->set_on_select(on_select_world);

  m_has_loaded = true;
}

void HomeMenu::update()
{
  using namespace entt::literals;

  if (!has_loaded())
  {
    return;
  }

  const auto delta = m_game_context.clock->delta;

  m_camera.update(delta);

  if (m_input_manager.poll_action("quit"_hs))
  {
    m_input_manager.quit();
  }
  else if (m_input_manager.poll_action("play"_hs))
  {
    ui::ItemList<WorldMetadata> worlds_metadata{};

    for (const auto& candidate : std::filesystem::directory_iterator(config::worlds_directory))
    {
      if (candidate.is_directory())
      {
        spdlog::debug("Loading world metadata from {}", candidate.path().filename().string());
        const auto world_metadata = serialization::load_world_metadata(candidate.path().filename());
        worlds_metadata.push_back({world_metadata, world_metadata.name});
      }
    }

    m_world_list->set_actions(worlds_metadata);
    m_world_list->show();

    // std::vector<WorldMetadata> worlds_metadata = load_worlds_metadata();
    // std::vector<string> world_names;
    //
    // for (const auto& world_metadata : worlds_metadata)
    // {
    //   world_names.push_back(world_metadata.name);
    // }

    // m_game_context.world_metadata = serialization::load_world_metadata("c253100c-43a0-ecea-354b-14ccedf7c40c");
    // m_game_context.scene_manager->push_scene<Gameplay>(m_game_context);
  }
  else if (m_input_manager.poll_action("create_world"_hs))
  {
    m_game_context.scene_manager->push_scene<WorldCreation>(m_game_context);
  }

  m_ui_manager.update();
}

void HomeMenu::render()
{
  using namespace entt::literals;

  if (!has_loaded())
  {
    return;
  }

  m_renderer.push_matrix("text"_hs, m_camera.view_matrix);
  m_renderer.batch("text"_hs, m_game_title, 60, 60, 0);
  m_renderer.batch("text"_hs, m_instructions, 60, 108, 0);
  m_ui_manager.render();
  m_renderer.render(m_camera);
  m_renderer.pop_matrix("text"_hs);
}
}  // namespace dl
