#pragma once

#include "./scene.hpp"
#include "core/input_manager.hpp"
#include "ecs/components/sprite.hpp"
#include "graphics/text.hpp"
#include "ui/types.hpp"
#include "ui/components/button_list.hpp"

namespace dl::ui
{
class WorldList;
}

namespace dl
{
struct GameContext;

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
  ui::ButtonList<MenuChoice>* m_button_list = nullptr;
  // Text m_instructions;
  Sprite m_typography;

  void m_load_worlds_metadata();
};
}  // namespace dl
