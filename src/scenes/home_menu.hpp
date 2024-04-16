#pragma once

#include "./scene.hpp"
#include "definitions.hpp"
#include "core/input_manager.hpp"
#include "ui/types.hpp"

namespace dl::ui
{
template <class T>
class TextButtonList;
class WorldList;

#ifdef DL_BUILD_DEBUG_TOOLS
class DemoWindow;
#endif
}  // namespace dl::ui

namespace dl::audio
{
struct SoundStreamSource;
}  // namespace dl::audio

namespace dl
{
struct GameContext;
class Texture;

class HomeMenu : public Scene
{
 public:
  HomeMenu(GameContext& game_context);

  void load() override;
  void init() override;
  void process_input();
  void update() override;
  void render() override;

 private:
  enum class MenuChoice
  {
    Play,
    NewWorld,
    Settings,
    Credits,
  };

  InputManager& m_input_manager = InputManager::get_instance();
  ui::ItemList<WorldMetadata> m_worlds_metadata{};
  ui::WorldList* m_world_list = nullptr;
  ui::TextButtonList<MenuChoice>* m_button_list = nullptr;
  Texture* m_typography = nullptr;
  audio::SoundStreamSource* m_background_music = nullptr;

  void m_load_worlds_metadata();
  void m_on_select_menu_option(MenuChoice choice);

#ifdef DL_BUILD_DEBUG_TOOLS
  ui::DemoWindow* m_ui_demo_window = nullptr;
#endif
};
}  // namespace dl
