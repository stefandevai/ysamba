#include "./home_menu.hpp"

#include <spdlog/spdlog.h>

#include "../graphics/camera.hpp"
#include "../graphics/renderer.hpp"
#include "../world/society/name_generator.hpp"

namespace dl
{
HomeMenu::HomeMenu(const std::string& scene_key, Camera& camera) : Scene(scene_key, camera) {}

void HomeMenu::load()
{
  Scene::load();

  const auto game_title = m_lua.get_variable<std::wstring>("game_title");
  const auto instructions = m_lua.get_variable<std::wstring>("instructions");

  m_game_title.set_text(game_title);
  m_game_title.set_font_name("font-1980");
  m_game_title.set_font_size(32);

  m_instructions.set_text(instructions);
  m_instructions.set_font_name("font-1980");
  m_instructions.set_font_size(32);
  m_has_loaded = true;
}

void HomeMenu::update(const double delta, SetSceneFunction set_scene)
{
  (void)delta;

  if (!has_loaded())
  {
    return;
  }

  if (m_input_manager->poll_action("quit"))
  {
    m_input_manager->quit();
  }
  else if (m_input_manager->poll_action("play"))
  {
    set_scene("gameplay", m_camera);
  }
  else if (m_input_manager->poll_action("create_world"))
  {
    set_scene("world_creation", m_camera);
  }
  else if (m_input_manager->poll_action("generate_name"))
  {
    auto name_generator = NameGenerator(4, 10);
    name_generator.load("nahuatl");
    const auto name = name_generator.generate();
    spdlog::critical("NAME: {}", name);
  }
}

void HomeMenu::render(Renderer& renderer)
{
  if (!has_loaded())
  {
    return;
  }

  renderer.init("text");
  renderer.batch("text", m_game_title, 60, 60, 0);
  renderer.batch("text", m_instructions, 60, 108, 0);
  renderer.finalize("text");
}
}  // namespace dl
