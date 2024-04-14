#pragma once

#include "./scene.hpp"
#include "audio/sound_source.hpp"
#include "core/input_manager.hpp"
#include "ui/types.hpp"

namespace dl::ui
{
template <class T>
class TextButtonList;
class WorldList;
}  // namespace dl::ui

namespace dl
{
struct GameContext;
class Texture;

class HomeMenu : public Scene
{
 public:
  HomeMenu(GameContext& game_context);

  void load() override;
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
  audio::SoundSource m_music_theme{};

  void m_load_worlds_metadata();
  void m_on_select_menu_option(MenuChoice choice);
};
}  // namespace dl
