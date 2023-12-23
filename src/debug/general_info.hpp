#pragma once

namespace dl
{
struct GameContext;

class GeneralInfo
{
 public:
  GeneralInfo(GameContext& camera);
  void update();

 private:
  GameContext& m_game_context;
  bool m_open = true;
};
}  // namespace dl
