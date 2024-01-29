#pragma once

#include "./scene.hpp"
#include "core/input_manager.hpp"
#include "graphics/text.hpp"

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
  void update() override;
  void render() override;

 private:
  InputManager& m_input_manager = InputManager::get_instance();
  ui::WorldList* m_world_list = nullptr;
  Text m_game_title;
  Text m_instructions;
};
}  // namespace dl
