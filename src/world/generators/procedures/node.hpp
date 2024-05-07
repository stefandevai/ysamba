#pragma once

namespace dl
{
struct TileProcedureData
{
  Chunk* chunk;
  Cell* cell;
  Vector3i* cell_position;
  Vector3i* padded_size;
  std::vector<BlockType>* terrain;
};

class TileProcedureNode
{
 public:
  TileProcedureNode() = default;
  virtual ~TileProcedureNode() = default;

  virtual void apply(TileProcedureData& data) = 0;
};
}
