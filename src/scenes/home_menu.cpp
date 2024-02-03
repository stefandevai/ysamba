#include "./home_menu.hpp"

#include <fmt/chrono.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <entt/core/hashed_string.hpp>
#include <i18n/i18n.hpp>

#include "constants.hpp"
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
  using namespace i18n::literals;

  Scene::load();

  m_renderer.add_batch("text"_hs, "default"_hs, 2);

  // const auto game_title = m_json.object["game_title"].get<std::string>();
  // const auto instructions = m_json.object["instructions"].get<std::string>();

  m_game_title.set_font_size(16);
  m_game_title.set_typeface("font-1980"_hs);
  m_game_title.set_text("title"_t);

  m_instructions.set_font_size(16);
  m_instructions.set_typeface("font-1980"_hs);
  m_instructions.set_text("instructions"_t);

  m_load_worlds_metadata();

  const auto on_select_world = [this](const WorldMetadata& world_metadata) {
    m_ui_manager.force_hide_all();
    m_game_context.world_metadata = world_metadata;
    m_game_context.scene_manager->push_scene<Gameplay>(m_game_context);
  };

  m_world_list = m_ui_manager.emplace<ui::WorldList>();
  m_world_list->set_on_select(on_select_world);
  m_world_list->set_actions(m_worlds_metadata);

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
    m_load_worlds_metadata();

    if (m_worlds_metadata.empty())
    {
      spdlog::warn("No worlds found");
      return;
    }

    m_world_list->set_actions(m_worlds_metadata);
    m_world_list->show();
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

void HomeMenu::m_load_worlds_metadata()
{
  m_worlds_metadata.clear();

  for (const auto& candidate : std::filesystem::directory_iterator(directory::worlds))
  {
    if (candidate.is_directory())
    {
      const auto world_metadata = serialization::load_world_metadata(candidate.path().filename());

      if (world_metadata.id.empty())
      {
        continue;
      }

      const auto updated_at_time_t = std::chrono::system_clock::to_time_t(world_metadata.updated_at);
      const auto label = fmt::format("{}\n({:%d/%m/%Y %H:%M})", world_metadata.name, fmt::localtime(updated_at_time_t));
      m_worlds_metadata.push_back({world_metadata, label});
    }
  }

  std::sort(m_worlds_metadata.begin(), m_worlds_metadata.end(), [](const auto& lhs, const auto& rhs) {
    return lhs.first.updated_at > rhs.first.updated_at;
  });
}
}  // namespace dl
