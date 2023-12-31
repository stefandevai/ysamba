#pragma once

namespace dl
{
class Gameplay;

class ChunkDebugger
{
 public:
  bool open = true;

  ChunkDebugger(Gameplay& render);
  void update();

 private:
  Gameplay& m_gameplay;
};
}  // namespace dl
