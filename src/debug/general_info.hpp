#pragma once

namespace dl
{
struct GameContext;

class GeneralInfo
{
 public:
  bool open = true;

  GeneralInfo(GameContext& camera);
  ~GeneralInfo();
  void update();
  void toggle() { open = !open; }

 private:
  GameContext& m_game_context;

  void m_render_usage_info();
};
}  // namespace dl
