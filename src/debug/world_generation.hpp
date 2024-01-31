#pragma once

namespace dl
{
class ChunkManager;

class WorldGeneration
{
 public:
  bool open = true;

  WorldGeneration(ChunkManager& chunk_manager);
  void update();

 private:
  ChunkManager& m_chunk_manager;
};
}  // namespace dl
