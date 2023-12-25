#pragma once

namespace dl
{
struct GameContext;

class GeneralInfo
{
 public:
  bool open = true;

  GeneralInfo(GameContext& camera);
  void update();
  void toggle() { open = !open; }

 private:
  GameContext& m_game_context;
};
}  // namespace dl
