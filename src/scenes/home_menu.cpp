#include "./home_menu.hpp"

#include <fmt/chrono.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <entt/core/hashed_string.hpp>
#include <glm/glm.hpp>
#include <i18n_keyval/i18n.hpp>

#include "constants.hpp"
#include "core/game_context.hpp"
#include "core/scene_manager.hpp"
#include "core/serialization.hpp"
#include "graphics/renderer/texture.hpp"
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

  m_typography = m_game_context.asset_manager->get<Texture>("ysamba-typography"_hs);

  m_load_worlds_metadata();

  const auto on_select_world = [this](const WorldMetadata& world_metadata) {
    m_world_list->force_hide();
    m_game_context.world_metadata = world_metadata;
    m_game_context.scene_manager->push_scene<Gameplay>(m_game_context);
  };

  m_world_list = m_ui_manager.emplace<ui::WorldList>();
  m_world_list->set_on_select(on_select_world);
  m_world_list->set_actions(m_worlds_metadata);

  m_button_list = m_ui_manager.emplace<ui::ButtonList<MenuChoice>>();

  m_button_list->set_items({
      {MenuChoice::Play, "Play"},
      {MenuChoice::NewWorld, "New World"},
      {MenuChoice::Settings, "Settings"},
      {MenuChoice::Credits, "Credits"},
  });

  m_button_list->position = Vector3i{75, 193, 0};
  m_button_list->button_size = Vector2i{200, 25};
  m_button_list->button_text_color.set(0xCCC1AFFF);

  m_button_list->set_on_select([this](const MenuChoice choice) {
    switch (choice)
    {
    case MenuChoice::Play:
    {
      m_load_worlds_metadata();

      if (m_worlds_metadata.empty())
      {
        spdlog::warn("No worlds found");
        return;
      }

      m_world_list->set_actions(m_worlds_metadata);
      m_world_list->show();
      break;
    }
    case MenuChoice::NewWorld:
    {
      m_game_context.scene_manager->push_scene<WorldCreation>(m_game_context);
      break;
    }
    case MenuChoice::Settings:
    case MenuChoice::Credits:
    default:
      break;
    }
  });

  m_has_loaded = true;
}

void HomeMenu::process_input()
{
  using namespace entt::literals;

  if (!m_input_manager.is_context("home_menu"_hs))
  {
    return;
  }

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
}

void HomeMenu::update()
{
  if (!has_loaded())
  {
    return;
  }

  process_input();

  const auto delta = m_game_context.clock->delta;

  m_camera.update(delta);
  m_renderer.clear_color(27, 36, 32);
  m_ui_manager.update();
}

void HomeMenu::render()
{
  using namespace entt::literals;

  if (!has_loaded())
  {
    return;
  }

  m_renderer.ui_pass.batch.texture(*m_typography, 60, 60, 0);
  m_ui_manager.render();
  m_renderer.render(m_camera);
}

void HomeMenu::m_load_worlds_metadata()
{
  m_worlds_metadata.clear();

  for (const auto& candidate : std::filesystem::directory_iterator(directory::worlds))
  {
    if (candidate.is_directory())
    {
      const auto world_metadata = serialization::load_world_metadata(candidate.path().filename().string());

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
